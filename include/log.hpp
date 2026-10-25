#ifndef LOG_HPP
#define LOG_HPP

#include <type_traits>

#include <spdlog/spdlog.h>

namespace chess {

spdlog::logger* logger();

}  // namespace chess

#define CHESS_LOG_AT(level_macro, ...)                                              \
  do {                                                                              \
    if (!std::is_constant_evaluated()) level_macro(::chess::logger(), __VA_ARGS__); \
  } while (0)

#define CHESS_TRACE(...) CHESS_LOG_AT(SPDLOG_LOGGER_TRACE, __VA_ARGS__)
#define CHESS_DEBUG(...) CHESS_LOG_AT(SPDLOG_LOGGER_DEBUG, __VA_ARGS__)
#define CHESS_INFO(...) CHESS_LOG_AT(SPDLOG_LOGGER_INFO, __VA_ARGS__)
#define CHESS_WARN(...) CHESS_LOG_AT(SPDLOG_LOGGER_WARN, __VA_ARGS__)
#define CHESS_ERROR(...) CHESS_LOG_AT(SPDLOG_LOGGER_ERROR, __VA_ARGS__)
#define CHESS_CRITICAL(...) CHESS_LOG_AT(SPDLOG_LOGGER_CRITICAL, __VA_ARGS__)

// used log entry of a function
#define CHESS_TRACE_FUNC() CHESS_TRACE("enter")

#endif  // LOG_HPP
