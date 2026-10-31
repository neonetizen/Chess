#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "fen.hpp"
#include "position.hpp"
#include "zobrist.hpp"

using namespace chess;

static_assert(zobrist::side() != 0, "keys are generated at compile time");

/*
 * every key that can be XORed into a hash must be nonzero and unique, or two positions differing only in those
 * features would collide on every probe
 *   1. collect every real key (skipping the zero None row and the zero no-castling entry)
 *   2. check none are zero, then sort and look for neighbours that match
 */
TEST(Zobrist, KeysAreNonzeroAndDistinct) {
  std::vector<Key> keys;
  for (int p = 1; p <= kColors * kTypes; ++p) {
    for (int sq = 0; sq < kSquares; ++sq) keys.push_back(zobrist::piece_square(Piece(p), Square(sq)));
  }
  for (int cr = 1; cr <= peel(CastlingRights::All); ++cr) keys.push_back(zobrist::castling(CastlingRights(cr)));
  for (int file = 0; file < 8; ++file) keys.push_back(zobrist::ep(make_square(file, 2)));
  keys.push_back(zobrist::side());

  EXPECT_EQ(std::ranges::count(keys, Key{0}), 0);
  std::ranges::sort(keys);
  EXPECT_EQ(std::ranges::adjacent_find(keys), keys.end());
}

TEST(Zobrist, NoneEntriesAreZero) {
  for (int sq = 0; sq < kSquares; ++sq) EXPECT_EQ(zobrist::piece_square(Piece::None, Square(sq)), 0U);
  EXPECT_EQ(zobrist::castling(CastlingRights::None), 0U);
  EXPECT_EQ(compute_key(Position{}), Position{}.key());
}

TEST(Zobrist, EpKeyDependsOnlyOnFile) {
  EXPECT_EQ(zobrist::ep(Square::E3), zobrist::ep(Square::E6));
  EXPECT_NE(zobrist::ep(Square::E3), zobrist::ep(Square::D3));
}

/*
 * put_piece/remove_piece update the keys incrementally; they must always equal a rebuild from scratch
 *   1. walk the squares with a fixed stride so pieces of every kind land all over the board
 *   2. put or remove at each step, comparing both keys against compute_key / compute_pawn_key
 *   3. clear the board and check it's back to the empty key
 */
TEST(Zobrist, IncrementalMatchesFromScratch) {
  Position pos;
  for (int i = 0; i < 2000; ++i) {
    const auto sq = Square((i * 37) % kSquares);
    if (pos.piece_on(sq) == Piece::None) {
      pos.put_piece(Piece(1 + (i % (kColors * kTypes))), sq);
    } else {
      pos.remove_piece(sq);
    }
    ASSERT_EQ(pos.key(), compute_key(pos)) << "step " << i;
    ASSERT_EQ(pos.pawn_key(), compute_pawn_key(pos)) << "step " << i;
  }
  for (int sq = 0; sq < kSquares; ++sq) {
    if (pos.piece_on(Square(sq)) != Piece::None) pos.remove_piece(Square(sq));
  }
  EXPECT_EQ(pos.key(), 0U);
  EXPECT_EQ(pos.pawn_key(), 0U);
}

TEST(Zobrist, PieceMovedAwayAndBackRestoresKey) {
  auto pos = Position::from_fen(kStartposFen);
  ASSERT_TRUE(pos);
  const Key start = pos->key();

  pos->remove_piece(Square::G1);
  pos->put_piece(Piece::WN, Square::F3);
  EXPECT_NE(pos->key(), start);
  pos->remove_piece(Square::F3);
  pos->put_piece(Piece::WN, Square::G1);
  EXPECT_EQ(pos->key(), start);
}

TEST(Zobrist, FenKeysMatchFromScratch) {
  constexpr std::string_view kFens[] = {
      kStartposFen,
      "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
      "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
      "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3",
  };
  for (const auto fen : kFens) {
    const auto pos = Position::from_fen(fen);
    ASSERT_TRUE(pos) << fen;
    EXPECT_EQ(pos->key(), compute_key(*pos)) << fen;
    EXPECT_EQ(pos->pawn_key(), compute_pawn_key(*pos)) << fen;
    EXPECT_TRUE(is_valid(*pos)) << fen;
  }
}

/*
 * changing any single part of the position (side to move, each castling subset, ep file) must change the key
 *   1. build positions that differ from each other in exactly one of those fields
 *   2. check every pair of keys differs
 */
TEST(Zobrist, EachStateFieldChangesKey) {
  constexpr std::string_view kCastling[] = {"-", "K",  "Q",  "KQ",  "k",  "Kk",  "Qk",  "KQk",
                                            "q", "Kq", "Qq", "KQq", "kq", "Kkq", "Qkq", "KQkq"};
  std::vector<Key> keys;
  for (const auto rights : kCastling) {
    const auto pos = Position::from_fen(std::string("r3k2r/8/8/8/8/8/8/R3K2R w ") + std::string(rights) + " - 0 1");
    ASSERT_TRUE(pos) << rights;
    keys.push_back(pos->key());
  }
  const auto black = Position::from_fen("r3k2r/8/8/8/8/8/8/R3K2R b - - 0 1");
  ASSERT_TRUE(black);
  keys.push_back(black->key());

  const auto no_ep = Position::from_fen("4k3/8/8/2pPp3/8/8/8/4K3 w - - 0 1");
  const auto ep_c  = Position::from_fen("4k3/8/8/2pPp3/8/8/8/4K3 w - c6 0 1");
  const auto ep_e  = Position::from_fen("4k3/8/8/2pPp3/8/8/8/4K3 w - e6 0 1");
  ASSERT_TRUE(no_ep && ep_c && ep_e);
  keys.push_back(no_ep->key());
  keys.push_back(ep_c->key());
  keys.push_back(ep_e->key());

  std::ranges::sort(keys);
  EXPECT_EQ(std::ranges::adjacent_find(keys), keys.end());
}

TEST(Zobrist, PawnKeySeesOnlyPawns) {
  const auto a = Position::from_fen("r3k2r/pp6/8/8/8/8/PP6/R3K2R w KQkq - 0 1");
  const auto b = Position::from_fen("4k3/pp6/8/8/8/2N5/PP6/4K3 b - - 0 1");
  const auto c = Position::from_fen("r3k2r/pp6/8/8/8/1P6/P7/R3K2R w KQkq - 0 1");
  ASSERT_TRUE(a && b && c);
  EXPECT_EQ(a->pawn_key(), b->pawn_key());
  EXPECT_NE(a->key(), b->key());
  EXPECT_NE(a->pawn_key(), c->pawn_key());
}

/*
 * after 1.e4 there's no black pawn that can take on e3, so "e3" and "-" are the same position and must hash the same
 */
TEST(Zobrist, DeadEpSquareIsDropped) {
  const auto with_ep = Position::from_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1");
  const auto without = Position::from_fen("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
  ASSERT_TRUE(with_ep && without);
  EXPECT_EQ(with_ep->ep_square(), Square::None);
  EXPECT_EQ(*with_ep, *without);
  EXPECT_EQ(to_fen(*with_ep), "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq - 0 1");
}

TEST(Zobrist, LiveEpSquareIsKept) {
  const auto with_ep = Position::from_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
  const auto without = Position::from_fen("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 3");
  ASSERT_TRUE(with_ep && without);
  EXPECT_EQ(with_ep->ep_square(), Square::F6);
  EXPECT_NE(with_ep->key(), without->key());
}
