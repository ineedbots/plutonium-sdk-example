#pragma once

namespace callbacks
{
	void on_dvar_init(void(__cdecl*)());
	void on_after_dvar_init(void(__cdecl*)());
	void on_game_init(void(__cdecl*)(int time, int restart));
	void on_game_shutdown(void(__cdecl*)(int freeScripts));
	void on_player_pre_connect(void(__cdecl*)(unsigned int clientNum));
	void on_player_connect(void(__cdecl*)(unsigned int clientNum));
	void on_player_disconnect(void(__cdecl*)(unsigned int clientNum));
	void on_scripts_load(void(__cdecl*)());
	void on_scripts_execute(void(__cdecl*)());
}
