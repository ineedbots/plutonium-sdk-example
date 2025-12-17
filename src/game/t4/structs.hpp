#pragma once

namespace game::t4
{
    enum con_channel_e
    {
        CON_CHANNEL_DONT_FILTER = 0x0,
        CON_CHANNEL_ERROR = 0x1,
        CON_CHANNEL_GAMENOTIFY = 0x2,
        CON_CHANNEL_BOLDGAME = 0x3,
        CON_CHANNEL_SUBTITLE = 0x4,
        CON_CHANNEL_OBITUARY = 0x5,
        CON_CHANNEL_LOGFILEONLY = 0x6,
        CON_CHANNEL_CONSOLEONLY = 0x7,
        CON_CHANNEL_GFX = 0x8,
        CON_CHANNEL_SOUND = 0x9,
        CON_CHANNEL_FILES = 0xA,
        CON_CHANNEL_DEVGUI = 0xB,
        CON_CHANNEL_PROFILE = 0xC,
        CON_CHANNEL_UI = 0xD,
        CON_CHANNEL_CLIENT = 0xE,
        CON_CHANNEL_SERVER = 0xF,
        CON_CHANNEL_SYSTEM = 0x10,
        CON_CHANNEL_PLAYERWEAP = 0x11,
        CON_CHANNEL_AI = 0x12,
        CON_CHANNEL_ANIM = 0x13,
        CON_CHANNEL_PHYS = 0x14,
        CON_CHANNEL_FX = 0x15,
        CON_CHANNEL_LEADERBOARDS = 0x16,
        CON_CHANNEL_PARSERSCRIPT = 0x17,
        CON_CHANNEL_SCRIPT = 0x18,
        CON_CHANNEL_COOPINFO = 0x19,
        CON_BUILTIN_CHANNEL_COUNT = 0x1A,
    };

    struct cmd_function_s
    {
        cmd_function_s *next;
        const char *name;
        const char *autoCompleteDir;
        const char *autoCompleteExt;
        void (__cdecl *function)();
    };
    ASSERT_STRUCT_OFFSET(cmd_function_s, next, 0x0);
    ASSERT_STRUCT_OFFSET(cmd_function_s, name, 0x4);
    ASSERT_STRUCT_OFFSET(cmd_function_s, autoCompleteDir, 0x8);
    ASSERT_STRUCT_OFFSET(cmd_function_s, autoCompleteExt, 0xC);
    ASSERT_STRUCT_OFFSET(cmd_function_s, function, 0x10);
    ASSERT_STRUCT_SIZE(cmd_function_s, 0x14);

    struct CmdArgs
    {
        int nesting;
        int localClientNum[8];
        int controllerIndex[8];
        int argc[8];
        const char **argv[8];
    };
    ASSERT_STRUCT_OFFSET(CmdArgs, nesting, 0x0);
    ASSERT_STRUCT_OFFSET(CmdArgs, localClientNum, 0x4);
    ASSERT_STRUCT_OFFSET(CmdArgs, controllerIndex, 0x24);
    ASSERT_STRUCT_OFFSET(CmdArgs, argc, 0x44);
    ASSERT_STRUCT_OFFSET(CmdArgs, argv, 0x64);
    ASSERT_STRUCT_SIZE(CmdArgs, 0x84);
}