#include <stdinc.hpp>
#include "clientcommand.hpp"

namespace clientcommand
{
	namespace
	{
	}

	void add(const char* name, void(__cdecl* cb)(int))
	{
		plugin::get()->get_interface()->client_command()->register_client_command(name, cb);
	}

	class component final : public component_interface
	{
	public:
		void on_startup() override
		{
		}
	};
}

REGISTER_COMPONENT(clientcommand::component)
