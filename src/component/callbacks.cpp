#include <stdinc.hpp>
#include "callbacks.hpp"

namespace callbacks
{
	namespace
	{
	}

	void on_dvar_init(void(*cb)())
	{
		plugin::get()->get_interface()->callbacks()->on_dvar_init(cb);
	}

	void on_after_dvar_init(void(*cb)())
	{
		plugin::get()->get_interface()->callbacks()->on_after_dvar_init(cb);
	}

	void on_game_init(void(*cb)(int, int))
	{
		plugin::get()->get_interface()->callbacks()->on_game_init(cb);
	}

	void on_game_shutdown(void(*cb)(int))
	{
		plugin::get()->get_interface()->callbacks()->on_game_shutdown(cb);
	}

	void on_player_pre_connect(void(*cb)(unsigned int))
	{
		plugin::get()->get_interface()->callbacks()->on_player_pre_connect(cb);
	}

	void on_player_connect(void(*cb)(unsigned int))
	{
		plugin::get()->get_interface()->callbacks()->on_player_connect(cb);
	}

	void on_player_disconnect(void(*cb)(unsigned int))
	{
		plugin::get()->get_interface()->callbacks()->on_player_disconnect(cb);
	}

	void on_scripts_load(void(*cb)())
	{
		plugin::get()->get_interface()->callbacks()->on_scripts_load(cb);
	}

	void on_scripts_execute(void(*cb)())
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
