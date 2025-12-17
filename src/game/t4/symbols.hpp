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


    WEAK symbol<void()> Cbuf_AddServerText_f{ 0x46C790, 0x52D8E0 };
    WEAK symbol<void(con_channel_e code, const char* fmt, ...)> Com_Printf{ 0x562490, 0x59A2C0 };


    WEAK symbol<CmdArgs> cmd_args{ 0x22236F8, 0x1F41670 };
    WEAK symbol<cmd_function_s*> cmd_functions{ 0x222377C, 0x1F416F4 };

    WEAK symbol<CmdArgs> sv_cmd_args{ 0x22469B0, 0x1F52938 };
}