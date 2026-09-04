add_library(chess_core STATIC
    position.cpp
    fen.cpp
)
target_include_directories(chess_core PUBLIC ${CMAKE_SOURCE_DIR}/include)
target_compile_features(chess_core PUBLIC cxx_std_20)

if(MSVC)
    target_compile_options(chess_core PRIVATE /W4)
else()
    target_compile_options(chess_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(chess main.cpp)
target_link_libraries(chess PRIVATE chess_core)