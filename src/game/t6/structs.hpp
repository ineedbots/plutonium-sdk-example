#pragma once

namespace game::t6
{
	enum errorParm_t
	{
		ERR_FATAL = 0x0,
		ERR_DROP = 0x1,
		ERR_FROM_STARTUP = 0x2,
		ERR_SERVERDISCONNECT = 0x3,
		ERR_DISCONNECT = 0x4,
		ERR_SCRIPT = 0x5,
		ERR_SCRIPT_DROP = 0x6,
		ERR_LOCALIZATION = 0x7,
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
}