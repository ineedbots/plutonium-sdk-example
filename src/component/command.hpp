#pragma once

namespace command
{
	void add(const char* name, void(__cdecl*)());
	void add_sv(const char* name, void(__cdecl*)());
}
