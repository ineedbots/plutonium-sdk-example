#pragma once

namespace game::iw5
{
	int Cmd_Argc();
	
	int SV_Cmd_Argc();


	WEAK symbol<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)> Cmd_AddCommandInternal{ 0x545DF0, 0x0 };
	WEAK symbol<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)> Cmd_AddServerCommandInternal{ 0x545800, 0x0 };
	WEAK symbol<const char*(int argIndex)> Cmd_Argv{ 0x467600, 0x0 };
	WEAK symbol<int()> Cmd_Argc_ADDONE{ 0x5486A0, 0x0 };
	WEAK symbol<void()> Cbuf_AddServerText_f{ 0x71D130, 0x0 };

	WEAK symbol<const char*(int)> SV_Cmd_Argv{ 0x5455D0, 0x0 };


	WEAK symbol<CmdArgs> sv_cmd_args{ 0x1CAA998, 0x0 };
}