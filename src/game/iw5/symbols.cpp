#include <stdinc.hpp>

namespace game::iw5
{
    int Cmd_Argc()
    {
        return Cmd_Argc_ADDONE() - 1;
    }

    int SV_Cmd_Argc()
    {
        return sv_cmd_args->argc[sv_cmd_args->nesting];
    }
}