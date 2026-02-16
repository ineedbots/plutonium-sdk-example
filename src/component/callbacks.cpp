#include <stdinc.hpp>
#include "callbacks.hpp"

namespace callbacks
{
	namespace
	{
	}

	void on_dvar_init(void(__cdecl* cb)())
	{
		plugin::get()->get_interface()->callbacks()->on_dvar_init(cb);
	}

	void on_after_dvar_init(void(__cdecl* cb)())
	{
		plugin::get()->get_interface()->callbacks()->on_after_dvar_init(cb);
	}

	void on_game_init(void(__cdecl* cb)(int, int))
	{
		plugin::get()->get_interface()->callbacks()->on_game_init(cb);
	}

	void on_game_shutdown(void(__cdecl* cb)(int))
	{
		plugin::get()->get_interface()->callbacks()->on_game_shutdown(cb);
	}

	void on_player_pre_connect(void(__cdecl* cb)(unsigned int))
	{
		plugin::get()->get_interface()->callbacks()->on_player_pre_connect(cb);
	}

	void on_player_connect(void(__cdecl* cb)(unsigned int))
	{
		plugin::get()->get_interface()->callbacks()->on_player_connect(cb);
	}

	void on_player_disconnect(void(__cdecl* cb)(unsigned int))
	{
		plugin::get()->get_interface()->callbacks()->on_player_disconnect(cb);
	}

	void on_scripts_load(void(__cdecl* cb)())
	{
		plugin::get()->get_interface()->callbacks()->on_scripts_load(cb);
	}

	void on_scripts_execute(void(__cdecl* cb)())
	{
		plugin::get()->get_interface()->callbacks()->on_scripts_execute(cb);
	}

	class component final : public component_interface
	{
	public:
		void on_startup() override
		{
		}
	};
}

REGISTER_COMPONENT(callbacks::component)
