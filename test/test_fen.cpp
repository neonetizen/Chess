#include <string_view>

#include <gtest/gtest.h>

#include "fen.hpp"
#include "position.hpp"

using namespace chess;

TEST(Fen, StartposPieces) {
  const auto pos = Position::from_fen(kStartposFen);
  ASSERT_TRUE(pos);
  EXPECT_EQ(pos->piece_on(Square::E1), Piece::WK);
  EXPECT_EQ(pos->piece_on(Square::D8), Piece::BQ);
  EXPECT_EQ(pos->piece_on(Square::E4), Piece::None);
  EXPECT_EQ(pos->pieces(Color::White, PieceType::Pawn), kBbRank2);
  EXPECT_EQ(pos->pieces(Color::Black, PieceType::Pawn), kBbRank7);
  EXPECT_EQ(pos->pieces(PieceType::Pawn), kBbRank2 | kBbRank7);
  EXPECT_EQ(pos->pieces(Color::White), kBbRank1 | kBbRank2);
  EXPECT_EQ(pos->occupied(), kBbRank1 | kBbRank2 | kBbRank7 | kBbRank8);
  EXPECT_EQ(pos->side_to_move(), Color::White);
  EXPECT_EQ(pos->castling(), CastlingRights::All);
  EXPECT_EQ(pos->ep_square(), Square::None);
  EXPECT_EQ(pos->halfmove_clock(), 0);
  EXPECT_EQ(pos->fullmove_number(), 1);
}

TEST(Fen, RoundTrip) {
  constexpr std::string_view kFens[] = {
      kStartposFen,
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
      "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
      "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3",
      "4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 40",
  };
  for (const auto fen : kFens) {
    const auto pos = Position::from_fen(fen);
    ASSERT_TRUE(pos) << fen;
    EXPECT_EQ(to_fen(*pos), fen);
    EXPECT_TRUE(is_valid(*pos)) << fen;
  }
}

TEST(Fen, CountersDefaultWhenMissing) {
  const auto pos = Position::from_fen("4k3/8/8/8/8/8/8/4K3 b -  -");
  ASSERT_TRUE(pos);
  EXPECT_EQ(pos->side_to_move(), Color::Black);
  EXPECT_EQ(pos->halfmove_clock(), 0);
  EXPECT_EQ(pos->fullmove_number(), 1);
}

TEST(Fen, CountersUpToTheirTypeMax) {
  const auto pos = Position::from_fen("4k3/8/8/8/8/8/8/4K3 w - - 255 65535");
  ASSERT_TRUE(pos);
  EXPECT_EQ(pos->halfmove_clock(), 255);
  EXPECT_EQ(pos->fullmove_number(), 65535);
}

TEST(Fen, AcceptsAdjacentDigits) {
  const auto split  = Position::from_fen("4k3/44/8/8/8/8/71/4K3 w - - 0 1");
  const auto normal = Position::from_fen("4k3/8/8/8/8/8/8/4K3 w - - 0 1");
  ASSERT_TRUE(split);
  ASSERT_TRUE(normal);
  EXPECT_EQ(*split, *normal);
}

TEST(Fen, RejectsMalformed) {
  constexpr std::string_view kBad[] = {
      "",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP w KQkq - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/8 w KQkq - 0 1",
      "rnbqkbnr/pppppppp/9/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
      "rnbqkbnrp/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
      "rnbqkbnr/ppppxppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR x KQkq - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkqK - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KX - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e9 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - -1 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 one",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 extra",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w",
      "rnbqkbnr/pppppppp/08/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
      "rnbqkbnr/pppppppp//8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR/ w KQkq - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBN w KQkq - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR W KQkq - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR white KQkq - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w -K - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w -- - 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq E3 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq e33 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq i3 0 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - +1 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 1.5 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0x1 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 99999999999999999999 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 256 1",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 65536",
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR\tw KQkq - 0 1",
  };
  for (const auto fen : kBad) {
    EXPECT_FALSE(Position::from_fen(fen)) << fen;
  }
}
