#pragma once

namespace scheduler
{
	template<typename T>
	concept coro_awaitable_convertible =
		std::convertible_to<T, std::chrono::milliseconds> ||
		std::convertible_to<T, std::string_view>;

	template<typename T>
	concept coro_awaitable_has_pt_member = requires(T t)
	{
		{ t._pt } -> std::convertible_to<void*>;
	};

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

	// why only this struct but not the others, C2027: use of undefined type
	struct coro_wait : std::suspend_always
	{
		coro_wait() = default;
		coro_wait(std::chrono::milliseconds delay) : _delay{ delay }
		{
		}

		void await_suspend(std::coroutine_handle<> h) const;

	private:
		std::chrono::milliseconds _delay{ 0 };
	};
	struct coro_self;

	template<typename T = void>
	struct coro_waittill;

	struct coro_promise
	{
		struct promise_type
		{
			struct promise_data
			{
				void endon(const char* endon)
				{
					endon_list.emplace(endon);
				}

				unsigned long long id() const
				{
					return _id;
				}

				promise_data()
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

				friend promise_type;
				friend coro_promise;
				friend coro_wait;
				friend coro_self;

				template<typename T>
				friend struct coro_waittill;
			};
			promise_data _data;

			void unhandled_exception() noexcept
			{
				_data.exception = std::current_exception();
			}

			coro_promise get_return_object()
			{
				return coro_promise{ this };
			}

			std::suspend_never initial_suspend() noexcept
			{
				return {};
			}

			void return_void() noexcept
			{
			}

			auto yield_value(auto) = delete;

			std::suspend_always final_suspend() noexcept
			{
				return {};
			}

			auto await_transform(std::chrono::milliseconds&& ms)
			{
				return coro_wait{ ms };
			}

			template<typename T = void>
			auto await_transform(std::string_view&& notify)
			{
				return coro_waittill<T>{ std::string{ notify } };
			}

			template<typename T>
				requires (!coro_awaitable_convertible<T>)
			auto await_transform(T&& t)
			{
				if constexpr (coro_awaitable_has_pt_member<T>)
				{
					t._pt = this;
				}

				return std::forward<T>(t);
			}
		};

		using handle = std::coroutine_handle<promise_type>;

		explicit coro_promise(promise_type* p) : _coro_handle{ handle::from_promise(*p) }
		{
		}

		coro_promise(coro_promise&& rhs) noexcept : _coro_handle{ std::exchange(rhs._coro_handle, nullptr) }
		{
		}

		coro_promise& operator=(coro_promise&& rhs) noexcept
		{
			if (this != &rhs)
			{
				_coro_handle = std::exchange(rhs._coro_handle, nullptr);
			}

			return *this;
		}

		~coro_promise()
		{
			if (_coro_handle)
			{
				_coro_handle.destroy();
				_coro_handle = {};
			}
		}

		coro_promise() = delete;
		coro_promise(const coro_promise&) = delete;
		coro_promise& operator=(const coro_promise&) = delete;

		bool is_waittill() const noexcept
		{
			assert(!done());
			return _coro_handle.promise()._data.is_waittill;
		}

		const std::unordered_set<std::string>& get_endon_list() const
		{
			assert(!done());
			return _coro_handle.promise()._data.endon_list;
		}

		bool is_waiting_for_waittill() const noexcept
		{
			assert(!done());
			assert(is_waittill());
			return _coro_handle.promise()._data.is_waiting_for_waittill;
		}

		void resolve_waittill(std::shared_ptr<std::any> result) const
		{
			assert(!done());
			assert(is_waittill());
			assert(is_waiting_for_waittill());

			_coro_handle.promise()._data.is_waiting_for_waittill = false;
			_coro_handle.promise()._data.waittill_result = std::move(result);
		}

		const std::string& get_waittill() const noexcept
		{
			assert(!done());
			assert(is_waittill());
			assert(is_waiting_for_waittill());
			return _coro_handle.promise()._data.waittill;
		}

		std::chrono::milliseconds get_delay() const noexcept
		{
			assert(!done());
			assert(!is_waittill());
			return _coro_handle.promise()._data.delay;
		}

		std::chrono::high_resolution_clock::time_point get_last_call() const noexcept
		{
			assert(!done());
			assert(!is_waittill());
			return _coro_handle.promise()._data.lastcall;
		}

		bool done() const noexcept
		{
			return !_coro_handle || _coro_handle.done() || _coro_handle.promise()._data.killed;
		}

		void resume() const
		{
			assert(!done());
			_coro_handle.resume();

			auto exception = _coro_handle.promise()._data.exception;
			if (exception)
			{
				_coro_handle.promise()._data.exception = nullptr;
				std::rethrow_exception(exception);
			}
		}

		void kill() const noexcept
		{
			_coro_handle.promise()._data.killed = true;
		}

		void cleanup()
		{
			// the handle gets moved when erased from a vector?
			this->~coro_promise();
		}

	private:
		handle _coro_handle{};
	};

	struct coro_frame : coro_wait
	{
		coro_frame() = default;
		coro_frame(std::chrono::milliseconds delay) = delete;
	};

	template<typename T>
	struct coro_waittill : std::suspend_always
	{
		coro_waittill() = default;
		coro_waittill(std::string waittill) : _waittill{ std::move(waittill) }
		{
		}

		void await_suspend(coro_promise::handle h)
		{
			_h = h;
			auto& promise = h.promise();

			promise._data.waittill = std::move(_waittill);
			promise._data.is_waiting_for_waittill = true;
			promise._data.is_waittill = true;
		}

		T await_resume() const
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
		coro_promise::handle _h{};
		std::string _waittill{};
	};

	struct coro_self : std::suspend_never
	{
		coro_promise::promise_type* _pt = nullptr; // cant be private, the concept will fail, even with friend
		coro_promise::promise_type::promise_data* await_resume() const
		{
			static_assert(coro_awaitable_has_pt_member<coro_self>);
			assert(_pt);
			return &_pt->_data;
		}
	};

	void coro_notify(const char* notify, std::shared_ptr<std::any> result);

	template<typename T = void>
	void coro_notify(const char* notify = "", std::conditional_t<std::is_void_v<T>, std::monostate, T>&& value = {})
	{
		if constexpr (std::is_void_v<T>)
		{
			coro_notify(notify, {});
		}
		else
		{
			coro_notify(notify, std::make_shared<std::any>(std::move(value)));
		}
	}

	// make sure if you are using lambda captures to store them locally to the coroutine before the first co_await
	void on_coro(std::function<coro_promise()>&& coroutine, thread thread = main, int priorty = 0);

	void on_frame(std::function<void()>&& callback, thread thread = main, int priorty = 0);
	void delay(std::function<void()>&& callback, std::chrono::milliseconds delay, thread thread = main, bool once = true, int priorty = 0);
	void once(std::function<void()>&& callback, thread thread = main, int priorty = 0);
	void until(std::function<evaluation()>&& callback, thread thread = main, int priorty = 0);
	void every(std::function<evaluation()>&& callback, std::chrono::milliseconds interval,
			thread thread = main, int priorty = 0);

	void error(const char* message, int level);
}
