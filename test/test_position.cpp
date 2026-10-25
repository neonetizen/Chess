#include <string_view>

#include <gtest/gtest.h>

#include "fen.hpp"
#include "position.hpp"

using namespace chess;

/*
 * squares attacked by each side in one position, including squares that look attacked but aren't
 *
 *   8  . . . . k . . .
 *   7  . . . . . . . .
 *   6  . . . . . . . .
 *   5  . . . p . . . .
 *   4  . . . . . . . Q
 *   3  . . N . . . . .
 *   2  . . . . . . . .
 *   1  R . . . K . . .
 */
TEST(IsAttacked, EachPieceTypeAndColor) {
  const auto pos = from_fen("4k3/8/8/3p4/7Q/2N5/8/R3K3 w - - 0 1");
  ASSERT_TRUE(pos);
  using enum Square;

  EXPECT_TRUE(is_attacked(*pos, C4, Color::Black));
  EXPECT_TRUE(is_attacked(*pos, E4, Color::Black));
  EXPECT_TRUE(is_attacked(*pos, D7, Color::Black));
  EXPECT_TRUE(is_attacked(*pos, F8, Color::Black));
  EXPECT_TRUE(is_attacked(*pos, D5, Color::White));
  EXPECT_TRUE(is_attacked(*pos, D8, Color::White));
  EXPECT_TRUE(is_attacked(*pos, A8, Color::White));
  EXPECT_TRUE(is_attacked(*pos, A4, Color::White));
}

TEST(IsAttacked, NotByPushesBlockedRaysOrTheOtherSide) {
  const auto pos = from_fen("4k3/8/8/3p4/7Q/2N5/8/R3K3 w - - 0 1");
  ASSERT_TRUE(pos);
  using enum Square;

  EXPECT_FALSE(is_attacked(*pos, D4, Color::Black));
  EXPECT_FALSE(is_attacked(*pos, C6, Color::Black));
  EXPECT_FALSE(is_attacked(*pos, E6, Color::Black));
  EXPECT_FALSE(is_attacked(*pos, G1, Color::White));
  EXPECT_FALSE(is_attacked(*pos, F8, Color::White));
  EXPECT_FALSE(is_attacked(*pos, H1, Color::Black));
}

TEST(IsValid, RejectsIllegalPositions) {
  constexpr std::string_view illegal[] = {
      "8/8/8/8/8/8/8/4K3 w - - 0 1",        "4k3/8/8/8/8/8/8/3KK3 w - - 0 1",    "4k2P/8/8/8/8/8/8/4K3 w - - 0 1",
      "4k3/8/8/8/8/8/8/4K3 w K - 0 1",      "4k3/8/8/8/3pP3/8/8/4K3 w - e3 0 1", "4k3/8/8/8/8/8/8/4K3 b - e3 0 1",
      "4k3/8/8/8/8/8/8/4K3 w - - 0 0",      "3kk3/8/8/8/8/8/8/4K3 w - - 0 1",    "4k3/8/8/8/8/8/8/p3K3 w - - 0 1",
      "4k3/8/8/8/8/8/8/4K2r w K - 0 1",     "4k3/8/8/8/8/8/8/R3K3 w K - 0 1",    "4k3/8/8/8/8/8/8/3K3R w K - 0 1",
      "r3k3/8/8/8/8/8/8/4K3 w k - 0 1",     "4k3/8/8/8/4p3/8/8/4K3 b - e3 0 1",  "4k3/8/8/8/4P3/4N3/8/4K3 b - e3 0 1",
      "4k3/8/8/8/4P3/8/4N3/4K3 b - e3 0 1", "4k3/8/8/8/8/8/8/4K2r b - - 0 1",    "4k3/3P4/8/8/8/8/8/4K3 w - - 0 1",
      "8/8/8/8/8/8/8/3kK3 w - - 0 1",
  };
  for (const auto fen : illegal) {
    const auto pos = from_fen(fen);
    ASSERT_TRUE(pos) << fen;
    EXPECT_FALSE(is_valid(*pos)) << fen;
  }
}

TEST(IsValid, AcceptsLegalEdgeCases) {
  constexpr std::string_view legal[] = {
      "4k3/8/8/8/8/8/8/4K2r w - - 0 1",
      "4k3/4p3/8/8/8/8/8/4R1K1 w - - 0 1",
      "8/8/8/8/8/8/3P4/4k2K b - - 0 1",
      "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1",
  };
  for (const auto fen : legal) {
    const auto pos = from_fen(fen);
    ASSERT_TRUE(pos) << fen;
    EXPECT_TRUE(is_valid(*pos)) << fen;
  }
}

TEST(IsValid, CatchesBoardBitboardMismatch) {
  auto extra_bit = from_fen(STARTPOS_FEN);
  ASSERT_TRUE(extra_bit);
  extra_bit->pieces[peel(Color::White)][peel(PieceType::Knight)] |= square_bb(Square::E4);
  EXPECT_FALSE(is_valid(*extra_bit));

  auto missing_bit = from_fen(STARTPOS_FEN);
  ASSERT_TRUE(missing_bit);
  missing_bit->board[peel(Square::E4)] = Piece::WN;
  EXPECT_FALSE(is_valid(*missing_bit));

  auto stale_occupancy = from_fen(STARTPOS_FEN);
  ASSERT_TRUE(stale_occupancy);
  stale_occupancy->occupancy[peel(Color::Black)] |= square_bb(Square::E4);
  EXPECT_FALSE(is_valid(*stale_occupancy));
}

TEST(IsValid, RejectsNegativeHalfmoveClock) {
  auto pos = from_fen(STARTPOS_FEN);
  ASSERT_TRUE(pos);
  pos->halfmove_clock = -1;
  EXPECT_FALSE(is_valid(*pos));
}

TEST(PutRemove, KeepBoardAndBitboardsInSync) {
  Position pos;
  put_piece(pos, Piece::BN, Square::C6);
  EXPECT_EQ(pos.board[peel(Square::C6)], Piece::BN);
  EXPECT_EQ(pos.pieces[peel(Color::Black)][peel(PieceType::Knight)], square_bb(Square::C6));
  EXPECT_EQ(pos.occupancy[peel(Color::Black)], square_bb(Square::C6));

  remove_piece(pos, Square::C6);
  EXPECT_EQ(pos, Position{});
}

#ifndef NDEBUG
TEST(PutRemoveDeathTest, AssertsOnBadSquares) {
  Position pos;
  put_piece(pos, Piece::WK, Square::E1);
  EXPECT_DEATH(put_piece(pos, Piece::WQ, Square::E1), "");
  EXPECT_DEATH(put_piece(pos, Piece::None, Square::E2), "");
  EXPECT_DEATH(remove_piece(pos, Square::E2), "");
}
#endif
