include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.18.0
)

set(INSTALL_GTEST OFF)
FetchContent_makeAvailable(googletest)

add_executable(unit_tests
    test_bitboard.cpp
    test_primitives.cpp
    test_debug.cpp
    test_fen.cpp
)

target_link_libraries(unit_tests PRIVATE chess_core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(unit_tests)