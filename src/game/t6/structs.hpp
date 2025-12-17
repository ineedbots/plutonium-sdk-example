#pragma once

namespace game::t6
{
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
}