#include <stdinc.hpp>

namespace game::t5
{
	int SV_Cmd_Argc()
	{
		return sv_cmd_args->argc[sv_cmd_args->nesting];
	}
}