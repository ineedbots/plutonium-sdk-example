#pragma once
#include <plutonium_sdk.hpp>

namespace gsc
{
	namespace function
	{
		void add(const char* name, void(__cdecl*)());
	}

	namespace method
	{
		using scr_entref_t = plutonium::sdk::types::entref;
		void add(const char* name, void(__cdecl*)(scr_entref_t));
	}
}
