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

        struct coro_task
        {
            scheduler::coro_promise promise;
            std::function<scheduler::coro_promise()> coroutine; // for lambda capture
            int priority;
        };

        std::array<std::mutex, scheduler::thread::count> coro_mutexes_;
        std::array<std::vector<std::pair<std::function<scheduler::coro_promise()>, int>>, scheduler::thread::count> coro_tasks_queue_; // task to be added to active vector
        std::array<std::vector<coro_task>, scheduler::thread::count> coro_tasks_; // active tasks vector
        std::array<std::vector<std::pair<std::string, std::shared_ptr<std::any>>>, scheduler::thread::count> coro_notify_queue_; // pending notifies

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

		void check_coroutines(const scheduler::thread thread_)
		{
			assert(thread_ < scheduler::thread::count);
			auto& coro_tasks = coro_tasks_[thread_];

			// check for new tasks to add
			const auto pop_a_task = [thread_]() -> std::optional<std::pair<std::function<scheduler::coro_promise()>, int>>
			{
				std::lock_guard _(coro_mutexes_[thread_]);
				auto& coro_task_queue = coro_tasks_queue_[thread_];

				if (coro_task_queue.empty())
				{
					return {};
				}

				auto coro_task = coro_task_queue.back();
				coro_task_queue.pop_back();
				return coro_task;
			};
			auto coro_task_opt = pop_a_task();

			auto needs_sorting = false;
			while (coro_task_opt.has_value())
			{
				needs_sorting = true;

				auto& [coro, priority] = coro_task_opt.value();

				// CP.51: Do not use capturing lambdas that are coroutines; they are not guaranteed to be safe across suspension points.
				// initial_suspend is suspend_never, so your coroutines can store the lambda captures into local variables before the first suspension point (their co_await)
				// this also sucks, cause the priorty is not respected until after the first suspension point, but its better than the alternative of not being able to use lambda captures at all
				auto promise = coro();
				if (!promise.done())
				{
					// is this bugged? even though the lambda is stored on the coro_task struct (which it is inside the coro_tasks vector), the lambda capture is still not safe to be used after a coroutine suspension point
					// the coroutine doesnt seem to properly restore the lambda capture (even though it still exists (as stated above)); is it not a feature of c++ or msvc missing it?
					coro_tasks.emplace_back(std::move(promise), std::move(coro), priority);
				}

				coro_task_opt = pop_a_task();
			}

			if (needs_sorting)
			{
				std::ranges::sort(coro_tasks, [](const coro_task& left, const coro_task& right) -> int
				{
					return left.priority > right.priority;
				});
			}


			// handle notifes
			const auto pop_a_notify = [thread_]() -> std::optional<std::pair<std::string, std::shared_ptr<std::any>>>
			{
				std::lock_guard _(coro_mutexes_[thread_]);
				auto& coro_notify_queue = coro_notify_queue_[thread_];

				if (coro_notify_queue.empty())
				{
					return {};
				}

				auto coro_notify = coro_notify_queue.back();
				coro_notify_queue.pop_back();
				return coro_notify;
			};
			auto coro_notify_opt = pop_a_notify();

			while (coro_notify_opt.has_value())
			{
				auto& [notify, result] = *coro_notify_opt;

				// apply the notify to each active task
				for (auto& coro_task : coro_tasks)
				{
					if (coro_task.promise.done())
					{
						continue;
					}

					// kill the coros who have the notify in their endon list
					const auto& endon_list = coro_task.promise.get_endon_list();
					if (std::ranges::find(endon_list, notify) != endon_list.end())
					{
						coro_task.promise.kill();
						continue;
					}

					// wake up coros that are waiting for the notify
					if (!coro_task.promise.is_waittill() || !coro_task.promise.is_waiting_for_waittill())
					{
						continue;
					}

					// a blank waittill will be woken by anything
					const auto& waittill = coro_task.promise.get_waittill();
					if (!waittill.empty() && waittill != notify)
					{
						continue;
					}

					coro_task.promise.resolve_waittill(result);
				}

				coro_notify_opt = pop_a_notify();
			}


			// run the coros that need to run, delete the completed ones
			std::erase_if(coro_tasks, [](coro_task& coro_task_) -> bool
			{
				auto& promise = coro_task_.promise;
				if (promise.done())
				{
					promise.cleanup();
					return true;
				}

				if (promise.is_waittill())
				{
					if (promise.is_waiting_for_waittill())
					{
						return false;
					}
				}
				else
				{
					if (std::chrono::high_resolution_clock::now() - promise.get_last_call() < promise.get_delay())
					{
						return false;
					}
				}

				promise.resume();
				if (promise.done())
				{
					promise.cleanup();
					return true;
				}

				return false;
			});
		}

		void execute_for_thread(const scheduler::thread thread_)
		{
			check_coroutines(thread_);
			check_for_errors(thread_);
			execute(thread_);
		}
	}

	void coro_wait::await_suspend(std::coroutine_handle<> h) const
	{
		auto& promise = std::coroutine_handle<coro_promise::promise_type>::from_address(h.address()).promise();

		promise._data.delay = _delay;
		promise._data.is_waittill = false;
		promise._data.lastcall = std::chrono::high_resolution_clock::now();
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

	void on_coro(std::function<coro_promise()>&& coroutine, thread thread_, int priority)
	{
		assert(thread_ < thread::count);

		std::lock_guard _(coro_mutexes_[thread_]);
		coro_tasks_queue_[thread_].emplace_back(std::move(coroutine), priority);
	}

	void coro_notify(const char* notify, std::shared_ptr<std::any> result)
	{
		for (auto thread_ = 0u; thread_ < thread::count; thread_++)
		{
			assert(thread_ < thread::count);

			std::lock_guard _(coro_mutexes_[thread_]);
			coro_notify_queue_[thread_].emplace_back(notify, result);
		}
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
