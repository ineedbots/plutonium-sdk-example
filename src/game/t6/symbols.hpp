#pragma once

namespace game::t6
{
	cmd_function_s* SV_Cmd_FindCommand(const char* cmdName);


	inline auto& Cmd_AddCommandInternal = pointer<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)>(SELECT(0x5B3070, 0x4DC2A0));
	inline auto& Cmd_AddServerCommandInternal = pointer<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)>(SELECT(0x4536B0, 0x615800));
	inline auto& Cmd_Argv = pointer<const char*(int argIndex)>(SELECT(0x5608F0, 0x6B3D40));
	inline auto& Cmd_Argc = pointer<int()>(SELECT(0x5EA650, 0x4BE4B0));
	inline auto& Cbuf_AddServerText_f = pointer<void()>(SELECT(0x909DA0, 0xA30D90));

	inline auto& SV_Cmd_Argv = pointer<const char*(int)>(SELECT(0x6DBF80, 0x520F20));
	inline auto& SV_Cmd_Argc = pointer<int()>(SELECT(0x5A81B0, 0x6BCAE0));


	inline auto& sv_cmd_functions = pointer<cmd_function_s*>(SELECT(0x2538B50, 0x250A6D0));
}