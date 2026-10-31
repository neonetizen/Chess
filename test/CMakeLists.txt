include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.18.0
)

set(INSTALL_GTEST OFF)
FetchContent_makeAvailable(googletest)

add_executable(unit_tests
    main.cpp
    test_bitboard.cpp
    test_primitives.cpp
    test_debug.cpp
    test_fen.cpp
    test_attacks.cpp
    test_position.cpp
    test_zobrist.cpp
)

target_link_libraries(unit_tests PRIVATE chess_core GTest::gtest)

include(GoogleTest)
gtest_discover_tests(unit_tests)