#include <stdinc.hpp>

#include <plutonium_sdk.hpp>
#include "plugin.hpp"
#include "loader/component_loader.hpp"
#include "game/game.hpp"

namespace plugin
{
	std::uint32_t plugin::plugin_version()
	{
		return PLUTONIUM_SDK_VERSION;
	}

	const char* plugin::plugin_name()
	{
		return "plutonium_plugin_base";
	}

	bool plugin::is_game_supported(plutonium::sdk::game)
	{
		return game::get_gamemode() != game::none;
	}

	void plugin::on_startup(plutonium::sdk::iinterface* interface_ptr, plutonium::sdk::game game)
	{
		this->interface_ = interface_ptr;
		this->game_ = game;

		component_loader::on_startup();
	}

	void plugin::on_shutdown()
	{
		component_loader::on_shutdown();
	}

	plutonium::sdk::iinterface* plugin::get_interface()
	{
		return this->interface_;
	}

	plutonium::sdk::game plugin::get_game()
	{
		return this->game_;
	}

	plugin* get()
	{
		static plugin instance;
		return &instance;
	}
}
