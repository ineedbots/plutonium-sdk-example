#include <stdinc.hpp>

namespace game::t4
{
	int Cmd_Argc()
	{
		return cmd_args.argc[cmd_args.nesting];
	}

	const char* Cmd_Argv(int param)
	{
		const char* result;
		auto function_addr = SELECT(0x439080, 0x435CE0);

		__asm
		{
			mov eax, param;
			call function_addr;
			mov result, eax;
		}

		return result;
	}

	cmd_function_s* Cmd_FindCommand(const char* name)
	{
		const auto function_addr = SELECT(0x55CC70, 0x594DB0);
		cmd_function_s* result = nullptr;

		__asm
		{
			mov esi, name;
			call function_addr;
			mov result, eax;
		}

		return result;
	}

	void Cmd_AddServerCommandInternal(const char* cmdName, void(__cdecl* function)(), cmd_function_s* allocedCmd)
	{
		static const auto function_addr = SELECT(0x55C440, 0x594510);

		__asm
		{
			mov edi, cmdName;
			push allocedCmd;
			push function;
			call function_addr;
			add esp, 8;
		}
	}

	void Cmd_AddCommandInternal(const char* cmdName, void(__cdecl* function)(), cmd_function_s* allocedCmd)
	{
		if (Cmd_FindCommand(cmdName))
		{
			if (function)
			{
				Com_Printf(CON_CHANNEL_SYSTEM, "Cmd_AddCommand: %s already defined\n", cmdName);
			}
			return;
		}

		allocedCmd->name = cmdName;
		allocedCmd->function = function;
		allocedCmd->next = cmd_functions;
		cmd_functions = allocedCmd;
	}

	int SV_Cmd_Argc()
	{
		return sv_cmd_args.argc[sv_cmd_args.nesting];
	}

	const char* SV_Cmd_Argv(int param)
	{
		const char* result;

		static DWORD func = SELECT(0x55C080, 0x594150);

		__asm
		{
			mov eax, param;
			call func;
			mov result, eax;
		}

		return result;
	}
}