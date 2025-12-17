#pragma once

#pragma warning(disable: 6011)
#pragma warning(disable: 6054)
#pragma warning(disable: 6387)
#pragma warning(disable: 26451)
#pragma warning(disable: 26812)
#pragma warning(disable: 28182)

#pragma warning(disable: 4324)
#pragma warning(disable: 4459)
#pragma warning(disable: 4611)
#pragma warning(disable: 4359)
#pragma warning(error: 4409)

#define DLL_EXPORT extern "C" __declspec(dllexport)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <vector>
#include <cassert>
#include <mutex>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <functional>
#include <regex>
#include <queue>
#include <unordered_set>
#include <filesystem>
#include <map>
#include <csetjmp>
#include <atlcomcli.h>
#include <variant>
#include <optional>
#include <Psapi.h>
#include <timeapi.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

#pragma comment(lib, "ntdll.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "urlmon.lib" )
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "Crypt32.lib")
#pragma comment(lib, "Winmm.lib")

using namespace std::literals;

#include <plutonium_sdk.hpp>
#include "plugin.hpp"
#include "loader/component_interface.hpp"
#include "loader/component_loader.hpp"
#include "game/game.hpp"
#include "utils/string.hpp"
#include "utils/memory.hpp"
#include "utils/hook.hpp"
#include "utils/helpers.hpp"
using con = utils::helpers::console;
