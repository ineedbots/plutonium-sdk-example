#include <stdinc.hpp>
#include "scheduler.hpp"

namespace scheduler
{
	namespace
	{
		struct task
		{
			std::function<scheduler::evaluation()> callback;
			std::chrono::milliseconds interval;
			std::chrono::high_resolution_clock::time_point last_call;
			int priority;
		};

		std::array<std::mutex, scheduler::thread::count> mutexes_;
		std::array<std::vector<task>, scheduler::thread::count> tasks_queue_; // tasks to be added to active vector
		std::array<std::vector<task>, scheduler::thread::count> tasks_; // active tasks vector

		std::mutex error_mutex_;
		using error_task_ = std::pair<std::string, int>;
		std::queue<error_task_> errors_;

		std::thread async_thread_;
		std::atomic<bool>(async_kill_) = false;

		bool get_next_error(const char** error_message, int* error_code, scheduler::thread thread_)
		{
			assert(thread_ < scheduler::thread::count);

			// should not Com_Error on non game thread
			if (thread_ == scheduler::thread::async)
			{
				return false;
			}

			error_task_ error;
			{
				std::lock_guard _(error_mutex_);
				if (errors_.empty())
				{
					return false;
				}

				error = errors_.front();
				errors_.pop();
			}

			*error_code = error.second;
			*error_message = utils::string::va("%s", error.first.c_str());

			return true;
		}

		void check_for_errors(scheduler::thread thread_)
		{
			const char* message;
			int level;

			if (get_next_error(&message, &level, thread_))
			{
				if (game::is_iw5())
				{
					game::iw5::Com_Error(static_cast<game::iw5::errorParm_t>(level), "%s", message);
				}
				else if (game::is_t4())
				{
					game::t4::Com_Error(static_cast<game::t4::errorParm_t>(level), "%s", message);
				}
				else if (game::is_t5())
				{
					game::t5::Com_Error(static_cast<game::t5::errorParm_t>(level), "%s", message);
				}
				else if (game::is_t6())
				{
					game::t6::Com_Error(static_cast<game::t6::errorParm_t>(level), "%s", message);
				}
			}
		}

		void execute(const scheduler::thread thread_)
		{
			assert(thread_ < scheduler::thread::count);
			auto& tasks = tasks_[thread_];

			auto needs_sorting = false;
			{
				std::lock_guard _(mutexes_[thread_]);
				auto& task_queue = tasks_queue_[thread_];

				if (!task_queue.empty())
				{
					needs_sorting = true;
					std::ranges::move(task_queue, std::back_inserter(tasks));
					task_queue.clear();
				}
			}

			if (needs_sorting)
			{
				std::ranges::sort(tasks, [](const task& left, const task& right) -> int
				{
					return left.priority > right.priority;
				});
			}

			std::erase_if(tasks, [](task& task_) -> bool
			{
				const auto now = std::chrono::high_resolution_clock::now();

				if (now - task_.last_call < task_.interval)
				{
					return false;
				}

				task_.last_call = now;

				return (task_.callback() == scheduler::evaluation::remove);
			});
		}

		namespace coro_
		{
			struct task
			{
				scheduler::coro::promise promise;
				std::function<scheduler::coro::promise()> coroutine; // for lambda capture
			};

			std::array<std::mutex, scheduler::thread::count> mutexes_;
			std::array<std::vector<std::function<scheduler::coro::promise()>>, scheduler::thread::count> tasks_queue_; // task to be added to active vector
			std::array<std::vector<std::pair<std::string, std::shared_ptr<std::any>>>, scheduler::thread::count> notify_queue_; // pending notifies
			std::array<DWORD, scheduler::thread::count> thread_ids_{};
			std::array<std::list<task>, scheduler::thread::count> tasks_; // active tasks

			bool process_notify(const scheduler::thread thread_, const std::string& notify, std::shared_ptr<std::any>& result)
			{
				assert(thread_ < scheduler::thread::count);

				auto processed_a_notify = false;

				// apply the notify to each active task
				for (const auto& task_ : tasks_[thread_])
				{
					auto& promise = task_.promise;
					if (promise.done())
					{
						continue;
					}

					// kill the coros who have the notify in their endon list
					const auto& endon_list = promise.get_endon_list();
					if (std::ranges::find(endon_list, notify) != endon_list.end())
					{
						processed_a_notify = true;
						promise.kill();
						continue;
					}

					// wake up coros that are waiting for the notify
					if (promise.running() || !promise.is_waittill() || !promise.is_waiting_for_waittill())
					{
						continue;
					}

					// a blank waittill will be woken by anything
					const auto& waittill = promise.get_waittill();
					if (!waittill.empty() && waittill != notify)
					{
						continue;
					}

					processed_a_notify = true;
					promise.resolve_waittill(result);
				}

				return processed_a_notify;
			}

			bool process_task(const scheduler::thread thread_, std::function<coro::promise()>&& coro)
			{
				assert(thread_ < scheduler::thread::count);

				// CP.51: Do not use capturing lambdas that are coroutines; they are not guaranteed to be safe across suspension points.
				// initial_suspend is suspend_never, so your coroutines can store the lambda captures into local variables before the first suspension point (their co_await)
				auto promise = coro();
				if (!promise.done())
				{
					// is this bugged? even though the lambda is stored on the coro_task struct (which it is inside the coro_tasks vector), the lambda capture is still not safe to be used after a coroutine suspension point
					// the coroutine doesnt seem to properly restore the lambda capture (even though it still exists (as stated above)); is it not a feature of c++ or msvc missing it?
					tasks_[thread_].emplace_back(std::move(promise), std::move(coro));
					return true;
				}

				promise.cleanup();
				return false;
			}

			void check_coroutines(const scheduler::thread thread_)
			{
				assert(thread_ < scheduler::thread::count);

				if (!thread_ids_[thread_])
				{
					thread_ids_[thread_] = GetCurrentThreadId();
				}

				const auto process_pending_tasks = [&thread_]() -> bool
				{
					const auto pop_a_task = [&thread_]() -> std::optional<std::function<coro::promise()>>
					{
						std::lock_guard _(mutexes_[thread_]);
						auto& coro_task_queue = tasks_queue_[thread_];

						if (coro_task_queue.empty())
						{
							return {};
						}

						auto coro_task = coro_task_queue.back();
						coro_task_queue.pop_back();
						return coro_task;
					};

					auto had_task = false;
					auto coro_task_opt = pop_a_task();
					while (coro_task_opt.has_value())
					{
						had_task = true;

						process_task(thread_, std::move(*coro_task_opt));

						coro_task_opt = pop_a_task();
					}

					return had_task;
				};

				const auto process_pending_notifies = [&thread_]()
				{
					const auto pop_a_notify = [&thread_]() -> std::optional<std::pair<std::string, std::shared_ptr<std::any>>>
					{
						std::lock_guard _(mutexes_[thread_]);
						auto& coro_notify_queue = notify_queue_[thread_];

						if (coro_notify_queue.empty())
						{
							return {};
						}

						auto coro_notify = coro_notify_queue.back();
						coro_notify_queue.pop_back();
						return coro_notify;
					};

					auto had_notify = false;
					auto coro_notify_opt = pop_a_notify();
					while (coro_notify_opt.has_value())
					{
						had_notify = true;

						process_notify(thread_, coro_notify_opt->first, coro_notify_opt->second);

						coro_notify_opt = pop_a_notify();
					}

					return had_notify;
				};

				process_pending_tasks();
				process_pending_notifies();

				// run the delay tasks
				auto count = tasks_[thread_].size();
				auto i = 0u;
				for (const auto& task_ : tasks_[thread_])
				{
					if (i++ >= count)
					{
						break;
					}

					auto& promise = task_.promise;
					if (promise.done())
					{
						continue;
					}

					assert(!promise.running());
					if (promise.is_waittill() || promise.is_frame_end())
					{
						continue;
					}

					if (std::chrono::high_resolution_clock::now() - promise.get_last_call() < promise.get_delay())
					{
						continue;
					}

					promise.resume();
				}

				// run the others
				while (true)
				{
					auto ran_something = false;
					for (const auto& task_ : tasks_[thread_])
					{
						auto& promise = task_.promise;
						if (promise.done())
						{
							continue;
						}

						assert(!promise.running());
						if (promise.is_waittill())
						{
							if (promise.is_waiting_for_waittill())
							{
								continue;
							}
						}
						else if (!promise.is_frame_end(false))
						{
							continue;
						}

						ran_something = true;
						promise.resume();
					}

					if (!ran_something)
					{
						break;
					}
				}

				// cleanup
				std::erase_if(tasks_[thread_], [](task& task_) -> bool
				{
					auto& promise = task_.promise;
					if (promise.done())
					{
						promise.cleanup();
						return true;
					}

					return false;
				});
			}
		}

		void execute_for_thread(const scheduler::thread thread_)
		{
			check_for_errors(thread_);
			coro_::check_coroutines(thread_);
			execute(thread_);
		}
	}

	namespace coro
	{
		void delay::await_suspend(std::coroutine_handle<> h)
		{
			auto& promise = promise::handle::from_address(h.address()).promise();

			promise._data.delay = _delay;
			promise._data.is_waittill = false;
			promise._data.lastcall = std::chrono::high_resolution_clock::now();
		}

		void on(std::function<promise()>&& coroutine, thread thread_)
		{
			assert(thread_ < thread::count);

			if (coro_::thread_ids_[thread_] == GetCurrentThreadId())
			{
				coro_::process_task(thread_, std::move(coroutine));
			}
			else
			{
				std::lock_guard _(coro_::mutexes_[thread_]);
				coro_::tasks_queue_[thread_].emplace_back(std::move(coroutine));
			}
		}

		void notify(const char* notify, std::shared_ptr<std::any> result, thread thread_)
		{
			auto currentThreadId = GetCurrentThreadId();

			const auto one_thread = [&currentThreadId, &notify, &result](thread thread_)
			{
				assert(thread_ < thread::count);
				if (coro_::thread_ids_[thread_] == currentThreadId)
				{
					coro_::process_notify(thread_, notify, result);
				}
				else
				{
					std::lock_guard _(coro_::mutexes_[thread_]);

					// queue too big... is the thread not running?
					if (coro_::notify_queue_[thread_].size() < 1024)
					{
						coro_::notify_queue_[thread_].emplace_back(notify, result);
					}
				}
			};

			if (thread_ == thread::count)
			{
				for (auto i = 0u; i < thread::count; i++)
				{
					one_thread(static_cast<scheduler::thread>(i));
				}
			}
			else
			{
				one_thread(thread_);
			}
		}

		void notify(const char* notifystr, thread thread_)
		{
			notify(notifystr, std::shared_ptr<std::any>{}, thread_);
		}
	}

	void every(std::function<evaluation()>&& callback, const std::chrono::milliseconds interval, const thread thread_, int priority)
	{
		assert(thread_ < thread::count);

		auto last_call = std::chrono::high_resolution_clock::now();

		std::lock_guard _(mutexes_[thread_]);
		tasks_queue_[thread_].emplace_back(std::move(callback), interval, last_call, priority);
	}

	void on_frame(std::function<void()>&& callback, const thread thread_, int priority)
	{
		every([callback = std::move(callback)]()
		{
			callback();
			return evaluation::reschedule;
		}, 0ms, thread_, priority);
	}

	void delay(std::function<void()>&& callback, const std::chrono::milliseconds delay, const thread thread_, bool once, int priority)
	{
		every([callback = std::move(callback), once]()
		{
			callback();

			if (once)
			{
				return evaluation::remove;
			}

			return evaluation::reschedule;
		}, delay, thread_, priority);
	}

	void once(std::function<void()>&& callback, const thread thread_, int priority)
	{
		delay(std::move(callback), 0ms, thread_, true, priority);
	}

	void until(std::function<evaluation()>&& callback, const thread thread_, int priority)
	{
		every(std::move(callback), 0ms, thread_, priority);
	}

	void error(const char* message, int level)
	{
		std::lock_guard _(error_mutex_);
		errors_.emplace(message, level);
	}

	class component final : public component_interface
	{
	public:
		void on_shutdown() override
		{
			async_kill_.store(true);

			if (async_thread_.joinable())
			{
				async_thread_.join();
			}
		}
		
		void on_startup() override
		{
			plugin::get()->get_interface()->scheduler()->on_frame([]()
			{
				execute_for_thread(thread::main);
			}, plutonium::sdk::interfaces::scheduler::thread::main);

			plugin::get()->get_interface()->scheduler()->on_frame([]()
			{
				execute_for_thread(thread::game);
			}, plutonium::sdk::interfaces::scheduler::thread::game);

			// async thread
			async_thread_ = std::thread([]()
			{
				while (!async_kill_.load())
				{
					execute_for_thread(thread::async);

					std::this_thread::sleep_for(16ms);
				}
			});

			async_thread_.detach();
		}
	};
}

REGISTER_COMPONENT(scheduler::component)
