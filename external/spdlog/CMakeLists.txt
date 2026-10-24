find_package(Threads REQUIRED)

add_library(spdlog STATIC
    src/spdlog.cpp
    src/stdout_sinks.cpp
    src/color_sinks.cpp
    src/file_sinks.cpp
    src/async.cpp
    src/cfg.cpp
    src/bundled_fmtlib_format.cpp
)
target_include_directories(spdlog SYSTEM PUBLIC include)
target_compile_definitions(spdlog PUBLIC SPDLOG_COMPILED_LIB)
target_link_libraries(spdlog PUBLIC Threads::Threads)
