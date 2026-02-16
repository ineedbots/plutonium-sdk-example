#pragma once

namespace game::iw5
{
	int Cmd_Argc();
	
	int SV_Cmd_Argc();


	inline auto& Cmd_AddCommandInternal = pointer<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)>(SELECT(0x545DF0, 0x0));
	inline auto& Cmd_AddServerCommandInternal = pointer<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)>(SELECT(0x545800, 0x0));
	inline auto& Cmd_Argv = pointer<const char*(int argIndex)>(SELECT(0x467600, 0x0));
	inline auto& Cmd_Argc_ADDONE = pointer<int()>(SELECT(0x5486A0, 0x0));
	inline auto& Cbuf_AddServerText_f = pointer<void()>(SELECT(0x71D130, 0x0));
	inline auto& Com_Error = pointer<void(errorParm_t code, const char* fmt, ...)>(SELECT(0x555450, 0x0));

	inline auto& SV_Cmd_Argv = pointer<const char*(int)>(SELECT(0x5455D0, 0x0));


	inline auto& sv_cmd_args = pointer<CmdArgs>(SELECT(0x1CAA998, 0x0));
}