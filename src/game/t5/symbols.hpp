#pragma once

namespace game::t5
{
	int SV_Cmd_Argc();


	inline auto& Cmd_AddCommandInternal = pointer<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)>(SELECT(0x6AD580, 0x661400));
	inline auto& Cmd_AddServerCommandInternal = pointer<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)>(SELECT(0x466930, 0x558290));
	inline auto& Cmd_Argv = pointer<const char*(int argIndex)>(SELECT(0x681060, 0x557360));
	inline auto& Cmd_Argc = pointer<int()>(SELECT(0x425EB0, 0x5E50C0));
	inline auto& Cbuf_AddServerText_f = pointer<void()>(SELECT(0x48F620, 0x651A30));
	inline auto& Com_Error = pointer<void(errorParm_t code, const char* fmt, ...)>(SELECT(0x627380, 0x651D90));

	inline auto& SV_Cmd_Argv = pointer<const char*(int)>(SELECT(0x618C60, 0x68C150));


	inline auto& sv_cmd_args = pointer<CmdArgs>(SELECT(0x355BD88, 0x243D208));
}
