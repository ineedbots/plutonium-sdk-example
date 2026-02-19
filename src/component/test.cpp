#include <stdinc.hpp>
#include "scheduler.hpp"
#include "callbacks.hpp"
#include "gsc.hpp"
#include "command.hpp"
#include "clientcommand.hpp"

namespace test
{
	namespace
	{
		scheduler::coro::promise coro_b()
		{
			auto* self = co_await scheduler::coro::self();
			self->endon("end me");
			co_await 5s;

			con::info("%llu sending test\n", self->id());
			scheduler::coro::notify("test");
			co_await 1s;

			con::info("%llu sending test2 5\n", self->id());
			scheduler::coro::notify<int>("test2", 5);
			co_await 1s;

			con::info("%llu sending test3 yo\n", self->id());
			scheduler::coro::notify<std::string>("test3", "yo");
			co_await 1s;

			con::info("%llu sending a\n", self->id());
			scheduler::coro::notify("a");
			co_await 1s;

			std::string awd = "awd";
			con::info("%llu sending %s\n", self->id(), awd.c_str());
			scheduler::coro::notify(awd.c_str());
			con::info("%llu done\n", self->id());

			while (1)
			{
				co_await 1s;

				con::info("%llu waiting to die\n", self->id());
			}
		}
		
		scheduler::coro::promise coro_a()
		{
			auto* self = co_await scheduler::coro::self();
			co_await scheduler::coro::waittill<void>("test");
			con::info("%llu recv test\n", self->id());

			auto i = co_await scheduler::coro::waittill<int>("test2");
			con::info("%llu recv test2 %d\n", self->id(), i);

			auto s = co_await scheduler::coro::waittill<std::string>("test3");
			con::info("%llu recv test3 %s\n", self->id(), s.c_str());

			co_await "a";
			con::info("%llu recv a\n", self->id());

			std::string awd = "awd";
			co_await awd;
			con::info("%llu recv %s\n", self->id(), awd.c_str());

			co_await scheduler::coro::delay{ 100ms };
			co_await 100ms;
			co_await scheduler::coro::frame();

			con::info("%llu done\n", self->id());
			co_await 5s;

			con::info("%llu sending end me\n", self->id());
			std::array<std::string, 5> arr{};
			arr[0] = "awoijdawoidjaiowjd";
			arr[1] = "90yrji095hjk";
			arr[2] = "rtyjmrskmtsrm";
			arr[3] = "j5eej5z";
			arr[4] = "jaetje5tajae5";

			scheduler::coro::notify<std::array<std::string, 5>>("end me", std::move(arr));
		}

		int count = 0;
		scheduler::coro::promise coro_only_one()
		{
			auto* self = co_await scheduler::coro::self();

			self->endon("only one");
			scheduler::coro::notify("only one");

			while (1)
			{
				co_await 1s;

				con::info("%llu there can only be one\n", self->id());

				if (count++ < 5)
				{
					scheduler::coro::on(coro_only_one);
				}
				else
				{
					scheduler::coro::notify("only one");
				}
			}
		}
	}

	class component final : public component_interface
	{
	public:
		void on_startup() override
		{
			scheduler::coro::on(coro_a);
			scheduler::coro::on(coro_b);

			std::string lol = "lol";
			scheduler::coro::on([lc_lol = lol]() -> scheduler::coro::promise
			{
				// make sure to get lambda captures by value into the stack of your coroutine before a suspention point (co_await)
				auto lol = lc_lol;

				auto* self = co_await scheduler::coro::self();
				auto i = co_await scheduler::coro::waittill<std::array<std::string, 5>>("end me");

				con::info("%llu recv end me and done %s\n", self->id(), lol.c_str());
				for (const auto& s : i)
				{
					con::info("%llu %s\n", self->id(), s.c_str());
				}
			});

			scheduler::coro::on(coro_only_one);

			scheduler::coro::on([]() -> scheduler::coro::promise
			{
				auto* self = co_await scheduler::coro::self();
				co_await 3s;

				for (auto i = 0; i < 10; i++)
				{
					co_await scheduler::coro::endframe{};
					scheduler::coro::notify<int>("did you get it", i);
				}

				scheduler::coro::on([]() -> scheduler::coro::promise
				{
					auto* self = co_await scheduler::coro::self();
					auto l = co_await scheduler::coro::waittill<std::string>("mee");
					con::info("%llu recv mee %s\n", self->id(), l.c_str());
				});

				con::info("%llu send mee\n", self->id());
				scheduler::coro::notify<std::string>("mee", "yooooo");
			});

			scheduler::coro::on([]() -> scheduler::coro::promise
			{
				auto* self = co_await scheduler::coro::self();

				for (;;)
				{
					auto i = co_await scheduler::coro::waittill<int>("did you get it");
					con::info("%llu recv did you get it %d\n", self->id(), i);
				}
			});

			const auto data_consume = []() -> scheduler::coro::promise
			{
				auto se = scheduler::coro::self();
				auto* self = co_await se;
				se._pt->_data;

				while (1)
				{
					auto s = co_await scheduler::coro::waittill<std::string>("data");
					con::info("%llu recv data %s\n", self->id(), s.c_str());
				}
			};
			scheduler::coro::on(data_consume, scheduler::main);
			scheduler::coro::on(data_consume, scheduler::main);
			scheduler::coro::on(data_consume, scheduler::async);
			scheduler::coro::on(data_consume, scheduler::async);

			scheduler::coro::on([]() -> scheduler::coro::promise
			{
				auto* self = co_await scheduler::coro::self();

				for (auto h = 0; h < 5; h++)
				{
					co_await 1s;
					con::info("%llu send data\n", self->id());

					for (auto i = 0; i < 5; i++)
					{
						co_await scheduler::coro::endframe{};
						scheduler::coro::notify<std::string>("data", utils::string::va("data %d %d %llu", i, h, self->id()));
					}
				}
			});

			scheduler::coro::on([]() -> scheduler::coro::promise
			{
				auto* self = co_await scheduler::coro::self();
				co_await 6s;

				con::info("%llu send bad data\n", self->id());
				scheduler::coro::notify<int>("bad data", 5);
			});

			scheduler::coro::on([]() -> scheduler::coro::promise
			{
				auto* self = co_await scheduler::coro::self();
				try
				{
					co_await scheduler::coro::waittill<std::string>("bad data");
				}
				catch (const std::exception& ex)
				{
					con::info("%llu %s\n", self->id(), ex.what());
				}
			});
			
			clientcommand::add("test", [](int clientNum)
			{
				con::info("client %d did clientcommand test\n", clientNum);
			});

			callbacks::on_game_init([](int level_time, int restart)
			{
				con::info("Game init'd at %d, restart %d\n", level_time, restart);
			});

			callbacks::on_game_shutdown([](int freeScripts)
			{
				con::info("Game shutdown'd, freeScripts: %d\n", freeScripts);
			});

			gsc::function::add("test_func", []()
			{
				con::info("test_func was called\n");
			});

			gsc::method::add("test_meth", [](gsc::method::scr_entref_t entref)
			{
				con::info("test_method was called on ent type %d, ent num %d\n", entref.classnum, entref.entnum);
			});

			if (game::is_iw5())
			{
				assert(game::is_mp());

				// game should init before we add commands
				callbacks::on_after_dvar_init([]()
				{
					command::add("test_func", []()
					{
						con::info("test_func was called with %d args %s\n", game::iw5::Cmd_Argc(), game::iw5::Cmd_Argv(0));
					});

					command::add_sv("test_func2", []()
					{
						con::info("test_func2 was called with %d args %s\n", game::iw5::SV_Cmd_Argc(), game::iw5::SV_Cmd_Argv(0));
					});
				});
			}

			if (game::is_t4())
			{
				callbacks::on_after_dvar_init([]()
				{
					command::add("test_func", []()
					{
						con::info("test_func was called with %d args %s\n", game::t4::Cmd_Argc(), game::t4::Cmd_Argv(0));
					});

					command::add_sv("test_func2", []()
					{
						con::info("test_func2 was called with %d args %s\n", game::t4::SV_Cmd_Argc(), game::t4::SV_Cmd_Argv(0));
					});
				});
			}

			if (game::is_t5())
			{
				callbacks::on_after_dvar_init([]()
				{
					command::add("test_func", []()
					{
						con::info("test_func was called with %d args %s\n", game::t5::Cmd_Argc(), game::t5::Cmd_Argv(0));
					});

					command::add_sv("test_func2", []()
					{
						con::info("test_func2 was called with %d args %s\n", game::t5::SV_Cmd_Argc(), game::t5::SV_Cmd_Argv(0));
					});
				});
			}

			if (game::is_t6())
			{
				callbacks::on_after_dvar_init([]()
				{
					command::add("test_func", []()
					{
						con::info("test_func was called with %d args %s\n", game::t6::Cmd_Argc(), game::t6::Cmd_Argv(0));
					});

					command::add_sv("test_func2", []()
					{
						con::info("test_func2 was called with %d args %s\n", game::t6::SV_Cmd_Argc(), game::t6::SV_Cmd_Argv(0));
					});
				});
			}

			class RuleOfFive {
			private:
				int* data;  // resource: dynamically allocated integer

			public:
				// Default constructor
				explicit RuleOfFive(int value = 0) : data(new int(value)) {
					con::info("Default constructor (value = %d)", *data);
				}

				// Destructor
				~RuleOfFive() {
					con::info("Destructor (data = %d)", data ? *data : 0);
					delete data;
				}

				// Copy constructor
				RuleOfFive(const RuleOfFive& other) : data(new int(*other.data)) {
					con::info("Copy constructor (from %d)", *other.data);
				}

				// Copy assignment operator
				RuleOfFive& operator=(const RuleOfFive& other) {
					con::info("Copy assignment (from %d to %d)", *other.data, *data);
					if (this != &other) {
						*data = *other.data;   // copy the value, not the pointer
					}
					return *this;
				}

				// Move constructor
				RuleOfFive(RuleOfFive&& other) noexcept : data(other.data) {
					con::info("Move constructor (from %d)", *other.data);
					other.data = nullptr;  // leave other in a valid but unspecified state
				}

				// Move assignment operator
				RuleOfFive& operator=(RuleOfFive&& other) noexcept {
					con::info("Move assignment (from %d to %d)",
						other.data ? *other.data : 0,
						data ? *data : 0);
					if (this != &other) {
						delete data;               // release current resource
						data = other.data;          // steal the pointer
						other.data = nullptr;       // leave other in a valid state
					}
					return *this;
				}

				// Simple getter (optional)
				int value() const { return data ? *data : 0; }
			};

			scheduler::coro::on([]() -> scheduler::coro::promise
			{
				auto* self = co_await scheduler::coro::self();
				co_await 10s;

				auto a = RuleOfFive(420);
				con::info("%llu sending ro5 %d\n", self->id(), a.value());
				scheduler::coro::notify("ro5", std::move(a));
			});

			scheduler::coro::on([]() -> scheduler::coro::promise
			{
				auto* self = co_await scheduler::coro::self();

				auto a = co_await scheduler::coro::waittill<RuleOfFive>("ro5");
				con::info("%llu recv ro5 %d\n", self->id(), a.value());
			});
		}
	};
}

REGISTER_COMPONENT(test::component)
