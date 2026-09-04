include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.18.0
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF)
FetchContent_makeAvailable(googletest)

add_executable(unit_tests
    test_bitboard.cpp
)

target_link_libraries(unit_tests PRIVATE chess_core GTest::gtest_main)
target_compile_options(unit_tests PRIVATE  -Wall -Wextra -Wpedantic)

include(GoogleTest)
gtest_discover_tests(unit_tests)