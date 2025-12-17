#pragma once

namespace game::iw5
{
	struct cmd_function_s
	{
		cmd_function_s *next;
		const char *name;
		void (__cdecl *function)();
	};
	ASSERT_STRUCT_OFFSET(cmd_function_s, next, 0x0);
	ASSERT_STRUCT_OFFSET(cmd_function_s, name, 0x4);
	ASSERT_STRUCT_OFFSET(cmd_function_s, function, 0x8);
	ASSERT_STRUCT_SIZE(cmd_function_s, 0xC);

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