#include <gtest/gtest.h>
#include "bitboard.hpp"
#include "primitives.hpp"

using namespace chess;

TEST(SquareBB, GivesOneBitAtCorrectIndex) {
    EXPECT_EQ(square_bb(Square::A1), 1ULL);
    EXPECT_EQ(square_bb(Square::H1), 0x80ULL);
    EXPECT_EQ(square_bb(Square::A8), 0x0100000000000000ULL);
}

TEST(Shift, DoesNotWrapEdges) {
    EXPECT_EQ(shift(square_bb(Square::H1), Direction::East), BB_NONE);
    EXPECT_EQ(shift(square_bb(Square::A4), Direction::West), BB_NONE);
    EXPECT_EQ(shift(square_bb(Square::H6), Direction::North), BB_NONE);
    EXPECT_EQ(shift(square_bb(Square::A8), Direction::South), BB_NONE);
}