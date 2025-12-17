#include <stdinc.hpp>

namespace game::iw5
{
    int Cmd_Argc()
    {
        return Cmd_Argc_ADDONE() - 1;
    }

    void Live_Frame(int localClientNum, int msec)
    {
        const auto addr = SELECT(0x5CBA10, 0x0);

        __asm
        {
            mov edi, localClientNum;
            push msec;
            call addr;
            add esp, 4;
        }
    }

    int SV_Cmd_Argc()
    {
        return sv_cmd_args->argc[sv_cmd_args->nesting];
    }
}