#pragma once

namespace scheduler
{
	enum thread : unsigned int
	{
		main,
		game,
		async,
		count
	};

	enum evaluation : bool
	{
		reschedule = false,
		remove = true
	};

	namespace coro
	{
		template<typename T>
		concept awaitable_convertible =
			std::convertible_to<T, std::chrono::milliseconds> ||
			std::convertible_to<T, std::string_view>;

		template<typename T>
		concept awaitable_has_pt_member = requires(T t)
		{
			{ t._pt } -> std::convertible_to<void*>;
		};

		// why only this struct but not the others, C2027: use of undefined type
		struct delay : std::suspend_always
		{
			delay() = default;
			delay(std::chrono::milliseconds delayms) : _delay{ delayms }
			{
			}

			void await_suspend(std::coroutine_handle<> h);

		private:
			std::chrono::milliseconds _delay{ 0 };
		};
		struct self;
		struct endframe;

		template<typename T = void>
		struct waittill;

		struct promise
		{
			struct promise_type
			{
				struct data
				{
					void endon(const char* endon)
					{
						endon_list.emplace(endon);
					}

					unsigned long long id() const
					{
						return _id;
					}

					data()
					{
						static std::atomic<unsigned long long> s_id = 0;
						_id = s_id++;
					}

				private:
					std::exception_ptr exception = nullptr;
					bool is_waittill = false;
					bool is_waiting_for_waittill = false;
					std::chrono::milliseconds delay{};
					std::string waittill{};
					std::shared_ptr<std::any> waittill_result{};
					std::unordered_set<std::string> endon_list{};
					std::chrono::high_resolution_clock::time_point lastcall;
					unsigned long long _id = 0;
					bool killed = false;
					bool is_frame_end = false;
					bool running = false;

					friend promise_type;
					friend promise;
					friend struct delay;
					friend self;
					friend endframe;

					template<typename T>
					friend struct waittill;
				};
				data _data;

				void unhandled_exception()
				{
					_data.exception = std::current_exception();
				}

				promise get_return_object()
				{
					return promise{ this };
				}

				std::suspend_never initial_suspend()
				{
					return {};
				}

				void return_void()
				{
				}

				auto yield_value(auto) = delete;

				std::suspend_always final_suspend() noexcept
				{
					return {};
				}

				auto await_transform(std::chrono::milliseconds ms)
				{
					return delay{ ms };
				}

				template<typename T = void>
				auto await_transform(std::string_view notify)
				{
					return waittill<T>{ std::string{ notify } };
				}

				template<typename T>
					requires (!awaitable_convertible<T>)
				auto await_transform(T&& t)
				{
					if constexpr (awaitable_has_pt_member<T>)
					{
						t._pt = this;
					}

					return std::forward<T>(t);
				}
			};

			using handle = std::coroutine_handle<promise_type>;

			explicit promise(promise_type* p) : handle_{ handle::from_promise(*p) }
			{
			}

			promise(promise&& rhs) noexcept : handle_{ std::exchange(rhs.handle_, nullptr) }
			{
			}

			promise& operator=(promise&& rhs) noexcept
			{
				if (this != &rhs)
				{
					handle_ = std::exchange(rhs.handle_, nullptr);
				}

				return *this;
			}

			~promise()
			{
				if (handle_)
				{
					handle_.destroy();
					handle_ = {};
				}
			}

			promise() = delete;
			promise(const promise&) = delete;
			promise& operator=(const promise&) = delete;

			bool done() const
			{
				return !handle_ || handle_.done() || handle_.promise()._data.killed;
			}

			void cleanup()
			{
				assert(done());

				// the handle gets moved when erased from a vector?
				this->~promise();
			}

			void kill() const
			{
				assert(!done());

				handle_.promise()._data.killed = true;
			}

			const std::unordered_set<std::string>& get_endon_list() const
			{
				assert(!done());

				return handle_.promise()._data.endon_list;
			}

			bool running() const
			{
				assert(!done());

				return handle_.promise()._data.running;
			}

			bool is_frame_end(bool peak = true) const
			{
				assert(!done());
				assert(!running());

				auto& promise = handle_.promise();
				auto answer = promise._data.is_frame_end;
				if (!peak)
				{
					promise._data.is_frame_end = false;
				}

				return answer;
			}

			bool is_waittill() const
			{
				assert(!done());
				assert(!running());

				return handle_.promise()._data.is_waittill && !is_frame_end();
			}

			bool is_waiting_for_waittill() const
			{
				assert(!done());
				assert(!running());
				assert(is_waittill());

				return handle_.promise()._data.is_waiting_for_waittill;
			}

			void resolve_waittill(std::shared_ptr<std::any> result) const
			{
				assert(!done());
				assert(!running());
				assert(is_waittill());
				assert(is_waiting_for_waittill());

				auto& promise = handle_.promise();
				promise._data.is_waiting_for_waittill = false;
				promise._data.waittill_result = std::move(result);
			}

			const std::string& get_waittill() const
			{
				assert(!done());
				assert(!running());
				assert(is_waittill());
				assert(is_waiting_for_waittill());

				return handle_.promise()._data.waittill;
			}

			std::chrono::milliseconds get_delay() const
			{
				assert(!done());
				assert(!running());
				assert(!is_waittill());
				assert(!is_frame_end());

				return handle_.promise()._data.delay;
			}

			std::chrono::high_resolution_clock::time_point get_last_call() const
			{
				assert(!done());
				assert(!running());
				assert(!is_waittill());
				assert(!is_frame_end());

				return handle_.promise()._data.lastcall;
			}

			void resume() const
			{
				assert(!done());
				assert(!running());

				handle_.promise()._data.running = true;
				handle_.resume();

				auto& promise = handle_.promise();
				promise._data.running = false;

				auto exception = promise._data.exception;
				if (exception)
				{
					promise._data.exception = nullptr;

					try
					{
						std::rethrow_exception(exception);
					}
					catch (const std::exception& e)
					{
						con::error("%llu coro unhandled_exception: %s\n", promise._data.id(), e.what());
					}
					catch (...)
					{
						con::error("%llu coro unhandled_exception (unknown type)\n", promise._data.id());
					}

					std::rethrow_exception(exception);
				}
			}

		private:
			handle handle_{};
		};

		struct frame : delay
		{
			frame() = default;
			frame(std::chrono::milliseconds delay) = delete;
		};

		struct endframe : std::suspend_always
		{
			void await_suspend(promise::handle h)
			{
				h.promise()._data.is_frame_end = true;
			}
		};

		template<typename T>
		struct waittill : std::suspend_always
		{
			waittill() = default;
			waittill(std::string waittillstr) : _waittill{ std::move(waittillstr) }
			{
			}

			void await_suspend(promise::handle h)
			{
				_h = h;
				auto& promise = h.promise();

				promise._data.waittill = std::move(_waittill);
				promise._data.is_waiting_for_waittill = true;
				promise._data.is_waittill = true;
			}

			T await_resume()
			{
				assert(_h);
				if constexpr (!std::is_void_v<T>)
				{
					auto& promise = _h.promise();
					assert(promise._data.waittill_result);

					auto result = std::any_cast<T>(*promise._data.waittill_result);
					promise._data.waittill_result = {};

					return result;
				}
			}

		private:
			coro::promise::handle _h{};
			std::string _waittill{};
		};

		struct self : std::suspend_never
		{
			promise::promise_type* _pt = nullptr; // cant be private, the concept will fail, even with friend
			promise::promise_type::data* await_resume()
			{
				static_assert(awaitable_has_pt_member<self>);
				assert(_pt);
				return &_pt->_data;
			}
		};

		void notify(const char* notifystr, std::shared_ptr<std::any> result, thread thread_);
		void notify(const char* notifystr = "", thread thread_ = count);

		template<typename T>
		void notify(const char* notifystr = "", T value = {}, thread thread_ = count)
		{
			notify(notifystr, std::make_shared<std::any>(std::move(value)), thread_);
		}

		// make sure if you are using lambda captures to store them locally to the coroutine before the first co_await
		void on(std::function<promise()>&& coroutine, thread thread = main);
	}

	void on_frame(std::function<void()>&& callback, thread thread = main, int priorty = 0);
	void delay(std::function<void()>&& callback, std::chrono::milliseconds delay, thread thread = main, bool once = true, int priorty = 0);
	void once(std::function<void()>&& callback, thread thread = main, int priorty = 0);
	void until(std::function<evaluation()>&& callback, thread thread = main, int priorty = 0);
	void every(std::function<evaluation()>&& callback, std::chrono::milliseconds interval,
			thread thread = main, int priorty = 0);

	void error(const char* message, int level);
}
