#include <stdinc.hpp>

#include "helpers.hpp"

namespace utils::helpers
{
    void console::info(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        std::string message = vfmt(fmt, args);
        va_end(args);

        plugin::get()->get_interface()->logging()->info(message);
    }

    void console::warn(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        std::string message = vfmt(fmt, args);
        va_end(args);

        plugin::get()->get_interface()->logging()->warn(message);
    }

    void console::error(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        std::string message = vfmt(fmt, args);
        va_end(args);

        plugin::get()->get_interface()->logging()->error(message);
    }

    std::string vfmt(const char* fmt, va_list args)
    {
        if (fmt == nullptr || *fmt == '\0')
        {
            return std::string{};
        }

        constexpr size_t SMALL_STRING_SIZE = 1024;
        char small_buf[SMALL_STRING_SIZE];

        va_list args_copy;
        va_copy(args_copy, args);
        int count = vsnprintf(small_buf, SMALL_STRING_SIZE, fmt, args_copy);
        va_end(args_copy);

        if (count < 0)
        {
            throw plutonium::sdk::exception("Failed to format string!");
        }

        if (static_cast<size_t>(count) < SMALL_STRING_SIZE)
        {
            return std::string{ small_buf, static_cast<size_t>(count) };
        }

        std::string buffer;
        buffer.resize(static_cast<size_t>(count) + 1);
        int result = vsnprintf(buffer.data(), count + 1, fmt, args);

        if (result < 0 || result > count)
        {
            throw plutonium::sdk::exception("Failed to format string!!");
        }

        buffer.resize(static_cast<size_t>(count));

        return buffer;
    }

    std::string fmt(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        std::string result = vfmt(fmt, args);
        va_end(args);

        return result;
    }

    std::string get_environment(const std::string& var)
    {
        char* env_value = nullptr;
        size_t env_value_size = 0;

        _dupenv_s(&env_value, &env_value_size, fmt("plutonium_%s", var.c_str()).c_str());

        if (env_value && env_value_size)
        {
            std::string str = std::string(env_value, env_value_size - 1);
            free(env_value);

            return str;
        }

        return std::string{};
    }

    std::filesystem::path get_plutonium_path()
    {
        const std::string key_result = get_environment("install_path");

        if (key_result.empty())
        {
            throw plutonium::sdk::exception("could not resolve Plutonium installation path.");
        }

        return key_result;
    }
}