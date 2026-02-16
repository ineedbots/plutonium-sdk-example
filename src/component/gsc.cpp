#include <stdinc.hpp>
#include "gsc.hpp"

namespace gsc
{
	namespace function
	{
		void add(const char* name, void(__cdecl* cb)())
		{
			plugin::get()->get_interface()->gsc()->register_function(name, cb);
		}
	}

	namespace method
	{
		void add(const char* name, void(__cdecl* cb)(scr_entref_t))
		{
			plugin::get()->get_interface()->gsc()->register_method(name, cb);
		}
	}

	class component final : public component_interface
	{
	public:
		void on_startup() override
		{
		}
	};
}

REGISTER_COMPONENT(gsc::component)
