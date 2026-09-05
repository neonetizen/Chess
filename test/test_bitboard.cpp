#include <gtest/gtest.h>
#include "bitboard.hpp"
#include "primitives.hpp"

using namespace chess;

TEST(SquareBB, GivesOneBitAtCorrectIndex) {
    EXPECT_EQ(square_bb(Square::A1), 0x0000000000000001ULL);
    EXPECT_EQ(square_bb(Square::H1), 0x0000000000000080ULL);
    EXPECT_EQ(square_bb(Square::A8), 0x0100000000000000ULL);
}

TEST(Shift, ShiftsToCorrectSquare) {
    EXPECT_EQ(shift(square_bb(Square::A1), Direction::North), square_bb(Square::A2));
    EXPECT_EQ(shift(square_bb(Square::A2), Direction::NorthEast), square_bb(Square::B3));
    EXPECT_EQ(shift(square_bb(Square::B3), Direction::East), square_bb(Square::D3));
    EXPECT_EQ(shift(square_bb(Square::D3), Direction::SouthEast), square_bb(Square::E2));
    EXPECT_EQ(shift(square_bb(Square::G6), Direction::South), square_bb(Square::G5));
    EXPECT_EQ(shift(square_bb(Square::G5), Direction::SouthWest), square_bb(Square::F4));
    EXPECT_EQ(shift(square_bb(Square::F4), Direction::West), square_bb(Square::F3));
    EXPECT_EQ(shift(square_bb(Square::F3), Direction::NorthWest), square_bb(Square::G2));
}
/*
TEST(Shift, DoesNotWrapEdges) {
    EXPECT_EQ(shift(square_bb(Square::H1), Direction::East), BB_NONE);
    EXPECT_EQ(shift(square_bb(Square::A4), Direction::West), BB_NONE);
    EXPECT_EQ(shift(square_bb(Square::H6), Direction::North), BB_NONE);
    EXPECT_EQ(shift(square_bb(Square::A8), Direction::South), BB_NONE);
}
*/