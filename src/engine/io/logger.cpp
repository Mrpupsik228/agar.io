#include "logger.h"

#include <iostream>
#include <cstdarg>

#define BRAINSTORM_LOG_BODY(PREFIX) va_list args;\
va_start(args, format);\
vprintf((std::string(PREFIX) + format + "\033[0m\n").c_str(), args);\
va_end(args)

namespace Brainstorm {
	void Logger::debug(const char* format, ...) {
		BRAINSTORM_LOG_BODY("\33[33;3m");
	}
	void Logger::info(const char* format, ...) {
		BRAINSTORM_LOG_BODY("\33[90m");
	}
	void Logger::warn(const char* format, ...) {
		BRAINSTORM_LOG_BODY("\33[43;30m");
	}
	void Logger::error(const char* format, ...) {
		BRAINSTORM_LOG_BODY("\33[31m");
	}
	void Logger::fatal(const char* format, ...) {
		BRAINSTORM_LOG_BODY("\33[41;30m");
		exit(1);
	}
	void Logger::notice(const char* format, ...) {
		BRAINSTORM_LOG_BODY("\33[102;30m");
	}
}