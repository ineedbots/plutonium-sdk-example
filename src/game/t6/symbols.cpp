#include <stdinc.hpp>

namespace game::t6
{
    cmd_function_s* SV_Cmd_FindCommand(const char* cmdName)
    {
        auto* func = *sv_cmd_functions;

        while (func)
        {
            if (!strcmp(func->name, cmdName))
            {
                return func;
            }

            func = func->next;
        }

        return nullptr;
    }
}