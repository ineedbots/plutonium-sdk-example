#pragma once

namespace utils::helpers
{
	class console final
	{
	public:
		static void info(const char* fmt, ...);
		static void warn(const char* fmt, ...);
		static void error(const char* fmt, ...);
	};

	std::string vfmt(const char* fmt, va_list args);
	std::string fmt(const char* fmt, ...);
	std::string get_environment(const std::string& var);
	std::filesystem::path get_plutonium_path();
}