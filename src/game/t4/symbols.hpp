#pragma once

namespace game::t4
{
	int Cmd_Argc();
	const char* Cmd_Argv(int index);
	void Cmd_AddServerCommandInternal(const char* cmdName, void(__cdecl* function)(), cmd_function_s* allocedCmd);
	void Cmd_AddCommandInternal(const char* cmdName, void(__cdecl* function)(), cmd_function_s* allocedCmd);
	cmd_function_s* Cmd_FindCommand(const char* name);
	
	int SV_Cmd_Argc();
	const char* SV_Cmd_Argv(int param);


	inline auto& Cbuf_AddServerText_f = pointer<void()>(SELECT(0x46C790, 0x52D8E0));
	inline auto& Com_Printf = pointer<void(con_channel_e code, const char* fmt, ...)>(SELECT(0x562490, 0x59A2C0));
	inline auto& Com_Error = pointer<void(errorParm_t code, const char* fmt, ...)>(SELECT(0x562DA0, 0x59AC50));


	inline auto& cmd_args = pointer<CmdArgs>(SELECT(0x22236F8, 0x1F41670));
	inline auto& cmd_functions = pointer<cmd_function_s*>(SELECT(0x222377C, 0x1F416F4));

	inline auto& sv_cmd_args = pointer<CmdArgs>(SELECT(0x22469B0, 0x1F52938));
}