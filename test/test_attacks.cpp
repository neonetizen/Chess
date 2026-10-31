#include <cstdint>
#include <cstdlib>

#include <gtest/gtest.h>

#include "attacks.hpp"
#include "bitboard.hpp"
#include "primitives.hpp"

using namespace chess;

namespace {

Bitboard squares(std::initializer_list<Square> list) {
  Bitboard bb = kBbNone;
  for (Square sq : list) bb = bb | sq;
  return bb;
}

}  // namespace

TEST(KnightAttacks, CenterAndCorners) {
  using enum Square;
  EXPECT_EQ(knight_attacks(E4), squares({D2, F2, C3, G3, C5, G5, D6, F6}));
  EXPECT_EQ(knight_attacks(A1), squares({B3, C2}));
  EXPECT_EQ(knight_attacks(H8), squares({G6, F7}));
  EXPECT_EQ(knight_attacks(G2), squares({E1, E3, F4, H4}));
  EXPECT_EQ(knight_attacks(B7), squares({D8, D6, A5, C5}));
}

TEST(KingAttacks, CenterAndCorners) {
  using enum Square;
  EXPECT_EQ(king_attacks(E4), squares({D3, E3, F3, D4, F4, D5, E5, F5}));
  EXPECT_EQ(king_attacks(A1), squares({A2, B2, B1}));
  EXPECT_EQ(king_attacks(H8), squares({G8, G7, H7}));
  EXPECT_EQ(king_attacks(H4), squares({H3, G3, G4, G5, H5}));
}

TEST(PawnAttacks, DiagonalsByColor) {
  using enum Square;
  EXPECT_EQ(pawn_attacks(Color::White, E4), squares({D5, F5}));
  EXPECT_EQ(pawn_attacks(Color::Black, E4), squares({D3, F3}));
  EXPECT_EQ(pawn_attacks(Color::White, A2), squares({B3}));
  EXPECT_EQ(pawn_attacks(Color::Black, H7), squares({G6}));
  EXPECT_EQ(pawn_attacks(Color::White, D8), kBbNone);
  EXPECT_EQ(pawn_attacks(Color::Black, D1), kBbNone);
}

/*
 * every table entry matches a brute-force answer
 *
 * 1. for each pair of squares, work out the file and rank distance
 * 2. a knight attacks at (1, 2) or (2, 1), a king at most 1 in each, a pawn 1 file over and 1 rank forward
 * 3. compare with the table bit for that pair
 */
TEST(Attacks, MatchBruteForceOnEverySquare) {
  for (int from = 0; from < kSquares; ++from) {
    for (int to = 0; to < kSquares; ++to) {
      const auto f  = Square(from);
      const auto t  = Square(to);
      const int df  = std::abs(file_of(t) - file_of(f));
      const int dr  = rank_of(t) - rank_of(f);
      const int adr = std::abs(dr);

      const bool knight = (df == 1 && adr == 2) || (df == 2 && adr == 1);
      const bool king   = from != to && df <= 1 && adr <= 1;
      const bool white  = df == 1 && dr == 1;
      const bool black  = df == 1 && dr == -1;

      EXPECT_EQ((knight_attacks(f) & t) != 0, knight) << "knight " << from << " -> " << to;
      EXPECT_EQ((king_attacks(f) & t) != 0, king) << "king " << from << " -> " << to;
      EXPECT_EQ((pawn_attacks(Color::White, f) & t) != 0, white) << "white pawn " << from << " -> " << to;
      EXPECT_EQ((pawn_attacks(Color::Black, f) & t) != 0, black) << "black pawn " << from << " -> " << to;
    }
  }
}

TEST(Attacks, UsableAtCompileTime) {
  static_assert(knight_attacks(Square::A1) == (square_bb(Square::B3) | square_bb(Square::C2)));
  static_assert(popcount(king_attacks(Square::E4)) == 8);
}

TEST(SliderAttacks, EmptyBoard) {
  using enum Square;
  EXPECT_EQ(rook_attacks(A1, kBbNone), (kBbFileA | kBbRank1) & ~square_bb(A1));
  EXPECT_EQ(rook_attacks(E4, kBbNone), (kBbFileE | kBbRank4) & ~square_bb(E4));
  EXPECT_EQ(bishop_attacks(A1, kBbNone), squares({B2, C3, D4, E5, F6, G7, H8}));
  EXPECT_EQ(bishop_attacks(D1, kBbNone), squares({C2, B3, A4, E2, F3, G4, H5}));
  EXPECT_EQ(queen_attacks(E4, kBbNone), rook_attacks(E4, kBbNone) | bishop_attacks(E4, kBbNone));
}

TEST(SliderAttacks, StopAtFirstBlockerAndIncludeIt) {
  using enum Square;
  EXPECT_EQ(rook_attacks(A1, squares({A3, C1, A6})), squares({A2, A3, B1, C1}));
  EXPECT_EQ(bishop_attacks(E4, squares({F5, C2, B7})), squares({F5, D3, C2, D5, C6, B7, F3, G2, H1}));
}

TEST(SliderAttacks, IgnoreOccupancyOffTheirRays) {
  using enum Square;
  const Bitboard off_rays = squares({B2, C3, G6, H7, D2});
  EXPECT_EQ(rook_attacks(E4, off_rays), rook_attacks(E4, kBbNone));
  EXPECT_EQ(bishop_attacks(E4, squares({E5, E3, D4, F4, A1})), bishop_attacks(E4, kBbNone));
}

TEST(SliderAttacks, DoNotWrapAroundEdges) {
  using enum Square;
  EXPECT_EQ(rook_attacks(H1, kBbNone) & squares({A2, A1}), squares({A1}));
  EXPECT_EQ(bishop_attacks(H4, kBbNone) & kBbFileA, kBbNone);
  EXPECT_EQ(bishop_attacks(A5, kBbNone) & kBbFileH, kBbNone);
}

/*
 * the magic/pext lookups agree with the slow ray walk for many blocker patterns on every square
 *
 * bad magic or a wrong mask shows up as a mismatch on some square with some occupancy
 * too many patterns to check exhaustively
 *
 * 1. for each square, build random occupancies of a few densities
 * 2. compare bishop and rook lookups against bishop_attacks_slow / rook_attacks_slow
 */
TEST(SliderAttacks, MatchSlowVersionOnRandomBoards) {
  std::uint64_t state = 0x2545F4914F6CDD1DULL;
  auto next           = [&state] {
    state ^= state << 13;
    state ^= state >> 7;
    state ^= state << 17;
    return state;
  };

  for (int s = 0; s < kSquares; ++s) {
    const auto sq = Square(s);
    for (int i = 0; i < 2000; ++i) {
      const Bitboard occupancies[] = {next() & next() & next(), next(), next() | next()};
      for (const Bitboard occupied : occupancies) {
        ASSERT_EQ(bishop_attacks(sq, occupied), detail::bishop_attacks_slow(sq, occupied)) << s;
        ASSERT_EQ(rook_attacks(sq, occupied), detail::rook_attacks_slow(sq, occupied)) << s;
      }
    }
  }
}

#ifndef NDEBUG
TEST(AttacksDeathTest, AssertsOnNoSquare) {
  EXPECT_DEATH(knight_attacks(Square::None), "");
  EXPECT_DEATH(king_attacks(Square::None), "");
  EXPECT_DEATH(pawn_attacks(Color::White, Square::None), "");
  EXPECT_DEATH(bishop_attacks(Square::None, kBbNone), "");
  EXPECT_DEATH(rook_attacks(Square::None, kBbNone), "");
}
#endif
