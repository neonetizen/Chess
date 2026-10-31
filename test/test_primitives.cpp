#include <gtest/gtest.h>

#include "position.hpp"
#include "primitives.hpp"

using namespace chess;

TEST(Square, FileAndRank) {
  EXPECT_EQ(file_of(Square::A1), 0);
  EXPECT_EQ(rank_of(Square::A1), 0);
  EXPECT_EQ(file_of(Square::E4), 4);
  EXPECT_EQ(rank_of(Square::E4), 3);
  EXPECT_EQ(make_square(7, 7), Square::H8);
  EXPECT_EQ(make_square(file_of(Square::C6), rank_of(Square::C6)), Square::C6);
}

TEST(Piece, MakeAndDecompose) {
  EXPECT_EQ(make_piece(Color::White, PieceType::Pawn), Piece::WP);
  EXPECT_EQ(make_piece(Color::White, PieceType::King), Piece::WK);
  EXPECT_EQ(make_piece(Color::Black, PieceType::Pawn), Piece::BP);
  EXPECT_EQ(make_piece(Color::Black, PieceType::King), Piece::BK);

  for (int c = 0; c < kColors; ++c) {
    for (int t = 0; t < kTypes; ++t) {
      const Piece p = make_piece(Color(c), PieceType(t));
      EXPECT_EQ(color_of(p), Color(c));
      EXPECT_EQ(type_of(p), PieceType(t));
    }
  }
}

TEST(Color, Flip) {
  EXPECT_EQ(~Color::White, Color::Black);
  EXPECT_EQ(~Color::Black, Color::White);
}

TEST(Castling, SetClearQuery) {
  CastlingRights rights = CastlingRights::All;
  rights &= ~CastlingRights::WhiteOO;
  EXPECT_FALSE(any(rights & CastlingRights::WhiteOO));
  EXPECT_TRUE(any(rights & (CastlingRights::WhiteOOO | CastlingRights::BlackOO)));

  rights &= ~(CastlingRights::BlackOO | CastlingRights::BlackOOO);
  EXPECT_EQ(rights, CastlingRights::WhiteOOO);

  rights |= CastlingRights::WhiteOO;
  EXPECT_EQ(rights, CastlingRights::WhiteOO | CastlingRights::WhiteOOO);
}

TEST(Castling, ComplementStaysInFourBits) {
  EXPECT_EQ(~CastlingRights::None, CastlingRights::All);
  EXPECT_EQ(~CastlingRights::All, CastlingRights::None);
}

TEST(Position, DefaultIsEmptyBoard) {
  const Position pos;
  EXPECT_EQ(pos.occupied(), kBbNone);
  EXPECT_EQ(pos.piece_on(Square::E1), Piece::None);
  EXPECT_EQ(pos.side_to_move(), Color::White);
  EXPECT_EQ(pos.castling(), CastlingRights::None);
  EXPECT_EQ(pos.ep_square(), Square::None);
  EXPECT_EQ(pos.fullmove_number(), 1);
}

TEST(Piece, FromChar) {
  EXPECT_EQ(piece_from_char('P'), Piece::WP);
  EXPECT_EQ(piece_from_char('k'), Piece::BK);
  for (const char c : {'x', 'X', '.', ' ', '0', '1', '/', '-', '\0'}) {
    EXPECT_EQ(piece_from_char(c), Piece::None) << "'" << c << "'";
  }
}

#ifndef NDEBUG
TEST(PrimitivesDeathTest, AssertsOnOutOfRange) {
  EXPECT_DEATH(make_square(8, 0), "");
  EXPECT_DEATH(make_square(0, -1), "");
  EXPECT_DEATH(type_of(Piece::None), "");
  EXPECT_DEATH(color_of(Piece::None), "");
}
#endif
