#include <stdinc.hpp>

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
	}

	class component final : public component_interface
	{
	public:
		void on_startup() override
		{
			if (game::is_iw5())
			{
				assert(game::is_mp());
				iw5::add_command("test_func", []()
				{
					con::info("test_func was called with %d args %s\n", game::iw5::Cmd_Argc(), game::iw5::Cmd_Argv(0));
				});

				iw5::add_sv_command("test_func2", []()
				{
					con::info("test_func2 was called with %d args %s\n", game::iw5::SV_Cmd_Argc(), game::iw5::SV_Cmd_Argv(0));
				});
			}
		}
	};
}

REGISTER_COMPONENT(test::component)
