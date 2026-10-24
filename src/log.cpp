#include <memory>

#include <spdlog/sinks/stdout_color_sinks.h>

#include "log.hpp"

namespace chess {

spdlog::logger* logger() {
  static const std::shared_ptr<spdlog::logger> chess_logger = [] {
    auto made = spdlog::stdout_color_mt("chess");
    made->set_pattern("[%H:%M:%S.%e] [%^%l%$] [%!] %v");
    made->set_level(spdlog::level::trace);
    return made;
  }();
  return chess_logger.get();
}

}  // namespace chess
