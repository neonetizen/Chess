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
  EXPECT_EQ(kBbNone | Square::E4, square_bb(Square::E4));
  EXPECT_EQ((kBbNone | Square::A1 | Square::H8) & Square::H8, square_bb(Square::H8));
  EXPECT_EQ(kBbRank1 & Square::A2, kBbNone);
}

TEST(Masks, FilesAndRanks) {
  EXPECT_EQ(kBbAll, 0xFFFFFFFFFFFFFFFFULL);
  EXPECT_EQ(kBbFileH, 0x8080808080808080ULL);
  EXPECT_EQ(kBbRank8, 0xFF00000000000000ULL);
  EXPECT_EQ(popcount(kBbFileC), 8);
  EXPECT_EQ(kBbFileD & kBbRank4, square_bb(Square::D4));
}

TEST(Masks, RankAndFileOfSquare) {
  EXPECT_EQ(rank_bb(Square::A1), kBbRank1);
  EXPECT_EQ(rank_bb(Square::H8), kBbRank8);
  EXPECT_EQ(file_bb(Square::E4), kBbFileE);
  EXPECT_EQ(file_bb(Square::H1), kBbFileH);
  EXPECT_EQ(rank_bb(Square::C5) & file_bb(Square::C5), square_bb(Square::C5));
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
  EXPECT_EQ(shift(square_bb(Square::H1), Direction::East), kBbNone);
  EXPECT_EQ(shift(square_bb(Square::A4), Direction::West), kBbNone);
  EXPECT_EQ(shift(square_bb(Square::H8), Direction::North), kBbNone);
  EXPECT_EQ(shift(square_bb(Square::A1), Direction::South), kBbNone);
  EXPECT_EQ(shift(square_bb(Square::H5), Direction::NorthEast), kBbNone);
  EXPECT_EQ(shift(square_bb(Square::H5), Direction::SouthEast), kBbNone);
  EXPECT_EQ(shift(square_bb(Square::A5), Direction::NorthWest), kBbNone);
  EXPECT_EQ(shift(square_bb(Square::A5), Direction::SouthWest), kBbNone);
}

TEST(Shift, WholeBoardLosesOneEdge) {
  EXPECT_EQ(shift(kBbAll, Direction::North), kBbAll & ~kBbRank1);
  EXPECT_EQ(shift(kBbAll, Direction::East), kBbAll & ~kBbFileA);
  EXPECT_EQ(shift(kBbAll, Direction::West), kBbAll & ~kBbFileH);
  EXPECT_EQ(shift(kBbAll, Direction::SouthWest), kBbAll & ~kBbFileH & ~kBbRank8);
}

TEST(Lsb, PopsSquaresLowToHigh) {
  Bitboard bb = kBbNone | Square::C1 | Square::E4 | Square::H8;
  EXPECT_EQ(popcount(bb), 3);
  EXPECT_EQ(pop_lsb(bb), Square::C1);
  EXPECT_EQ(pop_lsb(bb), Square::E4);
  EXPECT_EQ(pop_lsb(bb), Square::H8);
  EXPECT_EQ(bb, kBbNone);
}

#ifndef NDEBUG
TEST(BitboardDeathTest, AssertsOnNoSquare) {
  EXPECT_DEATH(square_bb(Square::None), "");
  EXPECT_DEATH(lsb(kBbNone), "");
  Bitboard empty = kBbNone;
  EXPECT_DEATH(pop_lsb(empty), "");
}
#endif

static_assert(shift(square_bb(Square::E4), Direction::North) == square_bb(Square::E5));
static_assert(lsb(kBbFileB) == Square::B1);
