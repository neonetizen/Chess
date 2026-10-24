#include <gtest/gtest.h>

#include "bitboard.hpp"
#include "primitives.hpp"

using namespace chess;

TEST(SquareBB, GivesOneBitAtCorrectIndex) {
  EXPECT_EQ(square_bb(Square::A1), 0x0000000000000001ULL);
  EXPECT_EQ(square_bb(Square::H1), 0x0000000000000080ULL);
  EXPECT_EQ(square_bb(Square::A8), 0x0100000000000000ULL);
  EXPECT_EQ(square_bb(Square::H8), 0x8000000000000000ULL);
}

TEST(SquareOps, OrAndUseTheSquareBit) {
  EXPECT_EQ(BB_NONE | Square::E4, square_bb(Square::E4));
  EXPECT_EQ((BB_NONE | Square::A1 | Square::H8) & Square::H8, square_bb(Square::H8));
  EXPECT_EQ(BB_RANK_1 & Square::A2, BB_NONE);
}

TEST(Masks, FilesAndRanks) {
  EXPECT_EQ(BB_ALL, 0xFFFFFFFFFFFFFFFFULL);
  EXPECT_EQ(BB_FILE_H, 0x8080808080808080ULL);
  EXPECT_EQ(BB_RANK_8, 0xFF00000000000000ULL);
  EXPECT_EQ(popcount(BB_FILE_C), 8);
  EXPECT_EQ(BB_FILE_D & BB_RANK_4, square_bb(Square::D4));
}

TEST(Shift, ShiftsToCorrectSquare) {
  EXPECT_EQ(shift(square_bb(Square::A1), Direction::North), square_bb(Square::A2));
  EXPECT_EQ(shift(square_bb(Square::A2), Direction::NorthEast), square_bb(Square::B3));
  EXPECT_EQ(shift(square_bb(Square::B3), Direction::East), square_bb(Square::C3));
  EXPECT_EQ(shift(square_bb(Square::D3), Direction::SouthEast), square_bb(Square::E2));
  EXPECT_EQ(shift(square_bb(Square::G6), Direction::South), square_bb(Square::G5));
  EXPECT_EQ(shift(square_bb(Square::G5), Direction::SouthWest), square_bb(Square::F4));
  EXPECT_EQ(shift(square_bb(Square::F4), Direction::West), square_bb(Square::E4));
  EXPECT_EQ(shift(square_bb(Square::F3), Direction::NorthWest), square_bb(Square::E4));
}

TEST(Shift, DoesNotWrapEdges) {
  EXPECT_EQ(shift(square_bb(Square::H1), Direction::East), BB_NONE);
  EXPECT_EQ(shift(square_bb(Square::A4), Direction::West), BB_NONE);
  EXPECT_EQ(shift(square_bb(Square::H8), Direction::North), BB_NONE);
  EXPECT_EQ(shift(square_bb(Square::A1), Direction::South), BB_NONE);
  EXPECT_EQ(shift(square_bb(Square::H5), Direction::NorthEast), BB_NONE);
  EXPECT_EQ(shift(square_bb(Square::H5), Direction::SouthEast), BB_NONE);
  EXPECT_EQ(shift(square_bb(Square::A5), Direction::NorthWest), BB_NONE);
  EXPECT_EQ(shift(square_bb(Square::A5), Direction::SouthWest), BB_NONE);
}

TEST(Shift, WholeBoardLosesOneEdge) {
  EXPECT_EQ(shift(BB_ALL, Direction::North), BB_ALL & ~BB_RANK_1);
  EXPECT_EQ(shift(BB_ALL, Direction::East), BB_ALL & ~BB_FILE_A);
  EXPECT_EQ(shift(BB_ALL, Direction::West), BB_ALL & ~BB_FILE_H);
  EXPECT_EQ(shift(BB_ALL, Direction::SouthWest), BB_ALL & ~BB_FILE_H & ~BB_RANK_8);
}

TEST(Lsb, PopsSquaresLowToHigh) {
  Bitboard bb = BB_NONE | Square::C1 | Square::E4 | Square::H8;
  EXPECT_EQ(popcount(bb), 3);
  EXPECT_EQ(pop_lsb(bb), Square::C1);
  EXPECT_EQ(pop_lsb(bb), Square::E4);
  EXPECT_EQ(pop_lsb(bb), Square::H8);
  EXPECT_EQ(bb, BB_NONE);
}

static_assert(shift(square_bb(Square::E4), Direction::North) == square_bb(Square::E5));
static_assert(lsb(BB_FILE_B) == Square::B1);
