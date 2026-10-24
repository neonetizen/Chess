#include <string_view>

#include <gtest/gtest.h>

#include "fen.hpp"
#include "position.hpp"

using namespace chess;

TEST(Fen, StartposPieces) {
  const auto pos = from_fen(STARTPOS_FEN);
  ASSERT_TRUE(pos);
  EXPECT_EQ(pos->board[peel(Square::E1)], Piece::WK);
  EXPECT_EQ(pos->board[peel(Square::D8)], Piece::BQ);
  EXPECT_EQ(pos->board[peel(Square::E4)], Piece::None);
  EXPECT_EQ(pos->pieces[peel(Color::White)][peel(PieceType::Pawn)], BB_RANK_2);
  EXPECT_EQ(pos->pieces[peel(Color::Black)][peel(PieceType::Pawn)], BB_RANK_7);
  EXPECT_EQ(pos->occupancy[peel(Color::White)], BB_RANK_1 | BB_RANK_2);
  EXPECT_EQ(pos->occupied(), BB_RANK_1 | BB_RANK_2 | BB_RANK_7 | BB_RANK_8);
  EXPECT_EQ(pos->side_to_move, Color::White);
  EXPECT_EQ(pos->castling, CastlingRights::All);
  EXPECT_EQ(pos->ep_square, Square::None);
  EXPECT_EQ(pos->halfmove_clock, 0);
  EXPECT_EQ(pos->fullmove_number, 1);
}

TEST(Fen, RoundTrip) {
  constexpr std::string_view fens[] = {
      STARTPOS_FEN,
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
      "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
      "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1",
      "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8",
      "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3",
      "4k3/8/8/8/3pP3/8/8/4K3 b - e3 0 40",
  };
  for (const auto fen : fens) {
    const auto pos = from_fen(fen);
    ASSERT_TRUE(pos) << fen;
    EXPECT_EQ(to_fen(*pos), fen);
    EXPECT_TRUE(is_valid(*pos)) << fen;
  }
}

TEST(Fen, CountersDefaultWhenMissing) {
  const auto pos = from_fen("4k3/8/8/8/8/8/8/4K3 b -  -");
  ASSERT_TRUE(pos);
  EXPECT_EQ(pos->side_to_move, Color::Black);
  EXPECT_EQ(pos->halfmove_clock, 0);
  EXPECT_EQ(pos->fullmove_number, 1);
}

TEST(Fen, RejectsMalformed) {
  constexpr std::string_view bad[] = {
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
  };
  for (const auto fen : bad) {
    EXPECT_FALSE(from_fen(fen)) << fen;
  }
}

TEST(IsValid, RejectsIllegalPositions) {
  constexpr std::string_view illegal[] = {
      "8/8/8/8/8/8/8/4K3 w - - 0 1",   "4k3/8/8/8/8/8/8/3KK3 w - - 0 1",    "4k2P/8/8/8/8/8/8/4K3 w - - 0 1",
      "4k3/8/8/8/8/8/8/4K3 w K - 0 1", "4k3/8/8/8/3pP3/8/8/4K3 w - e3 0 1", "4k3/8/8/8/8/8/8/4K3 b - e3 0 1",
      "4k3/8/8/8/8/8/8/4K3 w - - 0 0",
  };
  for (const auto fen : illegal) {
    const auto pos = from_fen(fen);
    ASSERT_TRUE(pos) << fen;
    EXPECT_FALSE(is_valid(*pos)) << fen;
  }
}

TEST(IsValid, CatchesBoardBitboardMismatch) {
  auto pos = from_fen(STARTPOS_FEN);
  ASSERT_TRUE(pos);
  pos->pieces[peel(Color::White)][peel(PieceType::Knight)] |= square_bb(Square::E4);
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
