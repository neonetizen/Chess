add_library(chess_core STATIC
    position.cpp
    fen.cpp
    debug.cpp
    log.cpp
)
target_include_directories(chess_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(chess_core PUBLIC cxx_std_20)
target_link_libraries(chess_core PUBLIC chess_options spdlog)

add_executable(chess main.cpp)
target_link_libraries(chess PRIVATE chess_core)
