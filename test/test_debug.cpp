#include <gtest/gtest.h>

#include "debug.hpp"

using namespace chess;

TEST(DebugString, Square) {
  EXPECT_EQ(to_string(Square::A1), "a1");
  EXPECT_EQ(to_string(Square::E4), "e4");
  EXPECT_EQ(to_string(Square::H8), "h8");
  EXPECT_EQ(to_string(Square::None), "-");
}

TEST(DebugString, CastlingRights) {
  EXPECT_EQ(to_string(CastlingRights::All), "KQkq");
  EXPECT_EQ(to_string(CastlingRights::None), "-");
  EXPECT_EQ(to_string(CastlingRights::WhiteOOO | CastlingRights::BlackOO), "Qk");
}

TEST(DebugString, Bitboard) {
  EXPECT_EQ(to_string(kBbNone | Square::A1 | Square::E4 | Square::H8),
            "8  . . . . . . . 1\n"
            "7  . . . . . . . .\n"
            "6  . . . . . . . .\n"
            "5  . . . . . . . .\n"
            "4  . . . . 1 . . .\n"
            "3  . . . . . . . .\n"
            "2  . . . . . . . .\n"
            "1  1 . . . . . . .\n"
            "\n"
            "   a b c d e f g h\n"
            "   0x8000000010000001\n");
}

TEST(DebugString, Position) {
  const auto pos = Position::from_fen("4k3/q7/8/8/8/8/3P4/4K3 b k d3 3 12");
  ASSERT_TRUE(pos);

  EXPECT_EQ(to_string(*pos),
            "8  . . . . k . . .\n"
            "7  q . . . . . . .\n"
            "6  . . . . . . . .\n"
            "5  . . . . . . . .\n"
            "4  . . . . . . . .\n"
            "3  . . . . . . . .\n"
            "2  . . . P . . . .\n"
            "1  . . . . K . . .\n"
            "\n"
            "   a b c d e f g h\n"
            "\n"
            "side: b  castling: k  ep: d3  halfmove: 3  fullmove: 12\n");
}
