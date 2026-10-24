cmake_minimum_required(VERSION 3.20)
project(chess CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(CHESS_NATIVE   "-march=native" ON)
option(CHESS_LTO      "LTO in release" ON)
option(CHESS_SANITIZE "asan + ubsan" OFF)

set(CHESS_LOG_LEVEL "" CACHE STRING "trace, debug, info, warn, error, critical or off")
if(NOT CHESS_LOG_LEVEL)
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        set(CHESS_LOG_LEVEL trace)
    else()
        set(CHESS_LOG_LEVEL info)
    endif()
endif()
string(TOUPPER ${CHESS_LOG_LEVEL} log_level)

add_library(chess_options INTERFACE)
target_compile_options(chess_options INTERFACE -Wall -Wconversion -Wshadow -Wimplicit-fallthrough)
target_compile_definitions(chess_options INTERFACE
    $<$<CONFIG:Debug>:_GLIBCXX_ASSERTIONS>
    SPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_${log_level}
)
if(CHESS_NATIVE)
    target_compile_options(chess_options INTERFACE -march=native)
endif()
if(CHESS_SANITIZE)
    target_compile_options(chess_options INTERFACE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(chess_options INTERFACE -fsanitize=address,undefined)
endif()

if(CHESS_LTO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
endif()

add_subdirectory(external/spdlog)
add_subdirectory(src)

enable_testing()
add_subdirectory(test)
