#include <stdinc.hpp>
#include "scheduler.hpp"
#include "callbacks.hpp"

namespace test
{
	namespace
	{
		namespace iw5
		{
			using namespace game::iw5;
			void add_command(const char* name, void(__cdecl* func)())
			{
				auto* func_store = utils::memory::allocate<cmd_function_s>();
				assert(func_store);
				Cmd_AddCommandInternal(name, func, func_store);
			}

			void add_sv_command(const char* name, void(__cdecl* func)())
			{
				add_command(name, Cbuf_AddServerText_f);

				auto* func_store = utils::memory::allocate<cmd_function_s>();
				assert(func_store);
				Cmd_AddServerCommandInternal(name, func, func_store);
			}
		}

		namespace t4
		{
			using namespace game::t4;
			void add_command(const char* name, void(__cdecl* func)())
			{
				auto* func_store = utils::memory::allocate<cmd_function_s>();
				assert(func_store);
				Cmd_AddCommandInternal(name, func, func_store);
			}

			void add_sv_command(const char* name, void(__cdecl* func)())
			{
				add_command(name, Cbuf_AddServerText_f);

				auto* func_store = utils::memory::allocate<cmd_function_s>();
				assert(func_store);
				Cmd_AddServerCommandInternal(name, func, func_store);
			}
		}

		namespace t5
		{
			using namespace game::t5;
			void add_command(const char* name, void(__cdecl* func)())
			{
				auto* func_store = utils::memory::allocate<cmd_function_s>();
				assert(func_store);
				Cmd_AddCommandInternal(name, func, func_store);
			}

			void add_sv_command(const char* name, void(__cdecl* func)())
			{
				add_command(name, Cbuf_AddServerText_f);

				auto* func_store = utils::memory::allocate<cmd_function_s>();
				assert(func_store);
				Cmd_AddServerCommandInternal(name, func, func_store);
			}
		}
		namespace t6
		{
			using namespace game::t6;
			void add_command(const char* name, void(__cdecl* func)())
			{
				auto* func_store = utils::memory::allocate<cmd_function_s>();
				assert(func_store);
				Cmd_AddCommandInternal(name, func, func_store);
			}

			void add_sv_command(const char* name, void(__cdecl* func)())
			{
				add_command(name, Cbuf_AddServerText_f);

				auto* func_store = utils::memory::allocate<cmd_function_s>();
				assert(func_store);
				Cmd_AddServerCommandInternal(name, func, func_store);
			}
		}

		scheduler::coro_promise coro_c()
		{
			auto* self = co_await scheduler::coro_self();
			auto i = co_await scheduler::coro_waittill<std::array<std::string, 5>>("end me");

			con::info("%llu recv end me and done\n", self->id());
			for (const auto& s : i)
			{
				con::info("%llu %s\n", self->id(), s.c_str());
			}
		}

		scheduler::coro_promise coro_b()
		{
			auto* self = co_await scheduler::coro_self();
			self->endon("end me");
			co_await 5s;

			con::info("%llu sending test\n", self->id());
			scheduler::coro_notify("test");
			co_await 1s;

			con::info("%llu sending test2 5\n", self->id());
			scheduler::coro_notify<int>("test2", 5);
			co_await 1s;

			con::info("%llu sending test3 yo\n", self->id());
			scheduler::coro_notify<std::string>("test3", "yo");
			co_await 1s;

			con::info("%llu sending a\n", self->id());
			scheduler::coro_notify("a");
			co_await 1s;

			std::string awd = "awd";
			con::info("%llu sending %s\n", self->id(), awd.c_str());
			scheduler::coro_notify(awd.c_str());
			con::info("%llu done\n", self->id());

			while (1)
			{
				co_await 1s;

				con::info("%llu waiting to die\n", self->id());
			}
		}
		
		scheduler::coro_promise coro_a()
		{
			auto* self = co_await scheduler::coro_self();
			co_await scheduler::coro_waittill<void>("test");
			con::info("%llu recv test\n", self->id());

			auto i = co_await scheduler::coro_waittill<int>("test2");
			con::info("%llu recv test2 %d\n", self->id(), i);

			auto s = co_await scheduler::coro_waittill<std::string>("test3");
			con::info("%llu recv test3 %s\n", self->id(), s.c_str());

			co_await "a";
			con::info("%llu recv a\n", self->id());

			std::string awd = "awd";
			co_await awd;
			con::info("%llu recv %s\n", self->id(), awd.c_str());

			co_await scheduler::coro_wait{ 100ms };
			co_await 100ms;
			co_await scheduler::coro_frame();

			con::info("%llu done\n", self->id());
			co_await 5s;

			con::info("%llu sending end me\n", self->id());
			std::array<std::string, 5> arr{};
			arr[0] = "awoijdawoidjaiowjd";
			arr[1] = "90yrji095hjk";
			arr[2] = "rtyjmrskmtsrm";
			arr[3] = "j5eej5z";
			arr[4] = "jaetje5tajae5";

			scheduler::coro_notify<std::array<std::string, 5>>("end me", std::move(arr));
		}
	}

	class component final : public component_interface
	{
	public:
		void on_startup() override
		{
			scheduler::on_coro(coro_a);
			scheduler::on_coro(coro_b);
			scheduler::on_coro(coro_c);

			callbacks::on_game_init([](int level_time, int restart)
			{
				con::info("Game init'd at %d, restart %d\n", level_time, restart);
			});

			callbacks::on_game_shutdown([](int freeScripts)
			{
				con::info("Game shutdown'd, freeScripts: %d\n", freeScripts);
			});

			if (game::is_iw5())
			{
				assert(game::is_mp());

				// game should init before we add commands
				plugin::get()->get_interface()->callbacks()->on_after_dvar_init([]()
				{
					iw5::add_command("test_func", []()
					{
						con::info("test_func was called with %d args %s\n", game::iw5::Cmd_Argc(), game::iw5::Cmd_Argv(0));
					});

					iw5::add_sv_command("test_func2", []()
					{
						con::info("test_func2 was called with %d args %s\n", game::iw5::SV_Cmd_Argc(), game::iw5::SV_Cmd_Argv(0));
					});
				});
			}

			if (game::is_t4())
			{
				plugin::get()->get_interface()->callbacks()->on_after_dvar_init([]()
				{
					t4::add_command("test_func", []()
					{
						con::info("test_func was called with %d args %s\n", game::t4::Cmd_Argc(), game::t4::Cmd_Argv(0));
					});

					t4::add_sv_command("test_func2", []()
					{
						con::info("test_func2 was called with %d args %s\n", game::t4::SV_Cmd_Argc(), game::t4::SV_Cmd_Argv(0));
					});
				});
			}

			if (game::is_t5())
			{
				plugin::get()->get_interface()->callbacks()->on_after_dvar_init([]()
				{
					t5::add_command("test_func", []()
					{
						con::info("test_func was called with %d args %s\n", game::t5::Cmd_Argc(), game::t5::Cmd_Argv(0));
					});

					t5::add_sv_command("test_func2", []()
					{
						con::info("test_func2 was called with %d args %s\n", game::t5::SV_Cmd_Argc(), game::t5::SV_Cmd_Argv(0));
					});
				});
			}

			if (game::is_t6())
			{
				plugin::get()->get_interface()->callbacks()->on_after_dvar_init([]()
				{
					t6::add_command("test_func", []()
					{
						con::info("test_func was called with %d args %s\n", game::t6::Cmd_Argc(), game::t6::Cmd_Argv(0));
					});

					t6::add_sv_command("test_func2", []()
					{
						con::info("test_func2 was called with %d args %s\n", game::t6::SV_Cmd_Argc(), game::t6::SV_Cmd_Argv(0));
					});
				});
			}
		}
	};
}

REGISTER_COMPONENT(test::component)
