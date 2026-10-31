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
  const auto pos = Position::from_fen("4k3/8/8/3p4/7Q/2N5/8/R3K3 w - - 0 1");
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
  const auto pos = Position::from_fen("4k3/8/8/3p4/7Q/2N5/8/R3K3 w - - 0 1");
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
  constexpr std::string_view kIllegal[] = {
      "8/8/8/8/8/8/8/4K3 w - - 0 1",        "4k3/8/8/8/8/8/8/3KK3 w - - 0 1",    "4k2P/8/8/8/8/8/8/4K3 w - - 0 1",
      "4k3/8/8/8/8/8/8/4K3 w K - 0 1",      "4k3/8/8/8/3pP3/8/8/4K3 w - e3 0 1", "4k3/8/8/8/8/8/8/4K3 b - e3 0 1",
      "4k3/8/8/8/8/8/8/4K3 w - - 0 0",      "3kk3/8/8/8/8/8/8/4K3 w - - 0 1",    "4k3/8/8/8/8/8/8/p3K3 w - - 0 1",
      "4k3/8/8/8/8/8/8/4K2r w K - 0 1",     "4k3/8/8/8/8/8/8/R3K3 w K - 0 1",    "4k3/8/8/8/8/8/8/3K3R w K - 0 1",
      "r3k3/8/8/8/8/8/8/4K3 w k - 0 1",     "4k3/8/8/8/4p3/8/8/4K3 b - e3 0 1",  "4k3/8/8/8/4P3/4N3/8/4K3 b - e3 0 1",
      "4k3/8/8/8/4P3/8/4N3/4K3 b - e3 0 1", "4k3/8/8/8/8/8/8/4K2r b - - 0 1",    "4k3/3P4/8/8/8/8/8/4K3 w - - 0 1",
      "8/8/8/8/8/8/8/3kK3 w - - 0 1",
  };
  for (const auto fen : kIllegal) {
    const auto pos = Position::from_fen(fen);
    ASSERT_TRUE(pos) << fen;
    EXPECT_FALSE(is_valid(*pos)) << fen;
  }
}

TEST(IsValid, AcceptsLegalEdgeCases) {
  constexpr std::string_view kLegal[] = {
      "4k3/8/8/8/8/8/8/4K2r w - - 0 1",
      "4k3/4p3/8/8/8/8/8/4R1K1 w - - 0 1",
      "8/8/8/8/8/8/3P4/4k2K b - - 0 1",
      "4k3/8/8/8/8/8/8/R3K3 w Q - 0 1",
  };
  for (const auto fen : kLegal) {
    const auto pos = Position::from_fen(fen);
    ASSERT_TRUE(pos) << fen;
    EXPECT_TRUE(is_valid(*pos)) << fen;
  }
}

TEST(PutRemove, KeepBoardAndBitboardsInSync) {
  Position pos;
  pos.put_piece(Piece::BN, Square::C6);
  EXPECT_EQ(pos.piece_on(Square::C6), Piece::BN);
  EXPECT_EQ(pos.pieces(Color::Black, PieceType::Knight), square_bb(Square::C6));
  EXPECT_EQ(pos.pieces(PieceType::Knight), square_bb(Square::C6));
  EXPECT_EQ(pos.pieces(Color::Black), square_bb(Square::C6));
  EXPECT_EQ(pos.occupied(), square_bb(Square::C6));

  pos.remove_piece(Square::C6);
  EXPECT_EQ(pos, Position{});
}

#ifndef NDEBUG
TEST(PutRemoveDeathTest, AssertsOnBadSquares) {
  Position pos;
  pos.put_piece(Piece::WK, Square::E1);
  EXPECT_DEATH(pos.put_piece(Piece::WQ, Square::E1), "");
  EXPECT_DEATH(pos.put_piece(Piece::None, Square::E2), "");
  EXPECT_DEATH(pos.remove_piece(Square::E2), "");
}
#endif
