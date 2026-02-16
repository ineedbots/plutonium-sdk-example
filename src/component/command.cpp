#include <stdinc.hpp>
#include "command.hpp"

namespace command
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
	}

	void add(const char* name, void(__cdecl* cb)())
	{
		if (game::is_iw5())
		{
			iw5::add_command(name, cb);
		}
		else if (game::is_t4())
		{
			t4::add_command(name, cb);
		}
		else if (game::is_t5())
		{
			t5::add_command(name, cb);
		}
		else if (game::is_t6())
		{
			t6::add_command(name, cb);
		}
	}

	void add_sv(const char* name, void(__cdecl* cb)())
	{
		if (game::is_iw5())
		{
			iw5::add_sv_command(name, cb);
		}
		else if (game::is_t4())
		{
			t4::add_sv_command(name, cb);
		}
		else if (game::is_t5())
		{
			t5::add_sv_command(name, cb);
		}
		else if (game::is_t6())
		{
			t6::add_sv_command(name, cb);
		}
	}

	class component final : public component_interface
	{
	public:
		void on_startup() override
		{
		}
	};
}

REGISTER_COMPONENT(command::component)
