#pragma once

namespace game::t6
{
	WEAK symbol<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)> Cmd_AddCommandInternal{ 0x5B3070, 0x4DC2A0 };
	WEAK symbol<void(const char *cmdName, void (__cdecl *function)(), cmd_function_s *allocedCmd)> Cmd_AddServerCommandInternal{ 0x4536B0, 0x615800 };
	WEAK symbol<const char*(int argIndex)> Cmd_Argv{ 0x5608F0, 0x6B3D40 };
	WEAK symbol<int()> Cmd_Argc{ 0x5EA650, 0x4BE4B0 };
	WEAK symbol<void()> Cbuf_AddServerText_f{ 0x909DA0, 0xA30D90 };

	WEAK symbol<const char*(int)> SV_Cmd_Argv{ 0x6DBF80, 0x520F20 };
	WEAK symbol<int()> SV_Cmd_Argc{ 0x5A81B0, 0x6BCAE0 };
}