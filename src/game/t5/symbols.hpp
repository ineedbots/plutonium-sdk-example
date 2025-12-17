#pragma once

namespace game::t5
{
	int SV_Cmd_Argc();


	WEAK symbol<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)> Cmd_AddCommandInternal{ 0x6AD580, 0x661400 };
	WEAK symbol<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)> Cmd_AddServerCommandInternal{ 0x466930, 0x558290 };
	WEAK symbol<const char*(int argIndex)> Cmd_Argv{ 0x681060, 0x557360 };
	WEAK symbol<int()> Cmd_Argc{ 0x425EB0, 0x5E50C0 };
	WEAK symbol<void()> Cbuf_AddServerText_f{ 0x48F620, 0x651A30 };

	WEAK symbol<const char*(int)> SV_Cmd_Argv{ 0x618C60, 0x68C150 };


	WEAK symbol<CmdArgs> sv_cmd_args{ 0x355BD88, 0x243D208 };
}