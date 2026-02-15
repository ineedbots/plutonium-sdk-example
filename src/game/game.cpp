#include <stdinc.hpp>
#include "utils/flags.hpp"
#include "game/game.hpp"
#include <utils/helpers.hpp>

namespace game
{
	std::optional<gamemode> current;

	gamemode get_gamemode()
	{
		if (!current.has_value())
		{
			// plugins can do this at crt init because the exe is loaded into memory before us
			current =
				reinterpret_cast<const char*>(0x88A5DC) == "CoDWaW.exe"s ? gamemode::t4sp : (
				reinterpret_cast<const char*>(0x858648) == "CoDWaW.exe"s ? gamemode::t4mp : (
				reinterpret_cast<const char*>(0xA60BA8) == "BlackOpsMP.exe"s ? gamemode::t5mp : (
				reinterpret_cast<const char*>(0x9D8514) == "BlackOps.exe"s ? gamemode::t5sp : (
				reinterpret_cast<const char*>(0x7F4CF4) == "iw5mp_ship.exe"s ? gamemode::iw5mp : (
				reinterpret_cast<const char*>(0xD369F2) == "t6zm.exe"s ? gamemode::t6zm : (
				reinterpret_cast<const char*>(0xD41402) == "t6mp.exe"s ? gamemode::t6mp : (
				gamemode::none
			)))))));
		}

		assert(current.has_value());
		return *current;
	}

	bool is_iw5()
	{
		return get_gamemode() == gamemode::iw5mp;
	}

	bool is_t4()
	{
		return get_gamemode() == gamemode::t4mp || get_gamemode() == gamemode::t4sp;
	}

	bool is_t5()
	{
		return get_gamemode() == gamemode::t5mp || get_gamemode() == gamemode::t5sp;
	}

	bool is_t6()
	{
		return get_gamemode() == gamemode::t6mp || get_gamemode() == gamemode::t6zm;
	}

	bool is_mp()
	{
		return get_gamemode() == gamemode::iw5mp || get_gamemode() == gamemode::t4mp ||
			get_gamemode() == gamemode::t5mp || get_gamemode() == gamemode::t6mp;
	}

	bool is_sp()
	{
		return !is_mp();
	}

	bool is_zm()
	{
		return is_sp();
	}

	bool is_dedi()
	{
		static bool answer = utils::flags::has_flag("dedicated");
		return answer;
	}

	bool is_lan()
	{
		static bool answer = utils::flags::has_flag("lan");
		return answer;
	}

	const std::filesystem::path& get_storage_location()
	{
		static const auto storage_path = utils::helpers::get_plutonium_path() / "storage" / (is_iw5() ? "iw5" : (is_t4() ? "t4" : (is_t5() ? "t5" : "t6")));
		return storage_path;
	}
}
