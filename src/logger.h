#pragma once

#include "logger.h"

#include <sstream>
#include <string_view>

#ifdef _WIN32
#include <Windows.h>
#endif

void logToFile(std::string_view text);

template <typename... Ts>
inline void log(Ts&&...args)
{
	std::ostringstream message;
	(message << ... << args) << '\n';

	const auto str = message.str();
	logToFile(str);

#ifdef _WIN32
	::OutputDebugStringA(str.c_str());
#endif
}