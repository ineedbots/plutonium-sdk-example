#pragma once

namespace callbacks
{
	void on_dvar_init(void(*)());
	void on_after_dvar_init(void(*)());
	void on_game_init(void(*)(int time, int restart));
	void on_game_shutdown(void(*)(int freeScripts));
	void on_player_pre_connect(void(*)(unsigned int clientNum));
	void on_player_connect(void(*)(unsigned int clientNum));
	void on_player_disconnect(void(*)(unsigned int clientNum));
	void on_scripts_load(void(*)());
	void on_scripts_execute(void(*)());
}
