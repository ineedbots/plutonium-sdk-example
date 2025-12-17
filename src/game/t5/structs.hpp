#pragma once

namespace game::t5
{
    struct cmd_function_s
    {
        cmd_function_s *next;
        const char *name;
        const char *autoCompleteDir;
        const char *autoCompleteExt;
        void (__cdecl *function)();
        bool consoleAccess;
    };
    ASSERT_STRUCT_OFFSET(cmd_function_s, next, 0x0);
    ASSERT_STRUCT_OFFSET(cmd_function_s, name, 0x4);
    ASSERT_STRUCT_OFFSET(cmd_function_s, autoCompleteDir, 0x8);
    ASSERT_STRUCT_OFFSET(cmd_function_s, autoCompleteExt, 0xC);
    ASSERT_STRUCT_OFFSET(cmd_function_s, function, 0x10);
    ASSERT_STRUCT_OFFSET(cmd_function_s, consoleAccess, 0x14);
    ASSERT_STRUCT_SIZE(cmd_function_s, 0x18);

    struct itemDef_s;
    struct CmdArgs
    {
        int nesting;
        int localClientNum[8];
        int controllerIndex[8];
        itemDef_s *itemDef[8];
        int argshift[8];
        int argc[8];
        const char **argv[8];
        char textPool[8192];
        const char *argvPool[512];
        int usedTextPool[8];
        int totalUsedArgvPool;
        int totalUsedTextPool;
    };
    ASSERT_STRUCT_OFFSET(CmdArgs, nesting, 0x0);
    ASSERT_STRUCT_OFFSET(CmdArgs, localClientNum, 0x4);
    ASSERT_STRUCT_OFFSET(CmdArgs, controllerIndex, 0x24);
    ASSERT_STRUCT_SIZE(CmdArgs, 0x28EC);
}