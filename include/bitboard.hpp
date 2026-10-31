#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include <bit>
#include <cassert>
#include <cstdint>

#include "primitives.hpp"

namespace chess {
/*
 * bit index = square index (A1 = bit 0, H1 = bit 7, A8 = bit 56, H8 = bit 63)
 *
 * printed as a board (rank 8 on top):
 *
 *   56 57 58 59 60 61 62 63   8
 *   48 49 50 51 52 53 54 55   7
 *   40 41 42 43 44 45 46 47   6
 *   32 33 34 35 36 37 38 39   5
 *   24 25 26 27 28 29 30 31   4
 *   16 17 18 19 20 21 22 23   3
 *    8  9 10 11 12 13 14 15   2
 *    0  1  2  3  4  5  6  7   1
 *
 *    A  B  C  D  E  F  G  H
 */
using Bitboard = std::uint64_t;

constexpr Bitboard kBbAll  = ~Bitboard{0};  // universal set
constexpr Bitboard kBbNone = 0;             // empty set

constexpr Bitboard kBbFileA = 0x0101010101010101ULL;
constexpr Bitboard kBbFileB = kBbFileA << 1;
constexpr Bitboard kBbFileC = kBbFileA << 2;
constexpr Bitboard kBbFileD = kBbFileA << 3;
constexpr Bitboard kBbFileE = kBbFileA << 4;
constexpr Bitboard kBbFileF = kBbFileA << 5;
constexpr Bitboard kBbFileG = kBbFileA << 6;
constexpr Bitboard kBbFileH = kBbFileA << 7;

constexpr Bitboard kBbRank1 = 0xFFULL;
constexpr Bitboard kBbRank2 = kBbRank1 << (8 * 1);
constexpr Bitboard kBbRank3 = kBbRank1 << (8 * 2);
constexpr Bitboard kBbRank4 = kBbRank1 << (8 * 3);
constexpr Bitboard kBbRank5 = kBbRank1 << (8 * 4);
constexpr Bitboard kBbRank6 = kBbRank1 << (8 * 5);
constexpr Bitboard kBbRank7 = kBbRank1 << (8 * 6);
constexpr Bitboard kBbRank8 = kBbRank1 << (8 * 7);

// bitboard with single square
constexpr Bitboard square_bb(Square sq) {
  assert(sq != Square::None);
  return Bitboard{1} << peel(sq);
}

constexpr Bitboard rank_bb(Square sq) { return kBbRank1 << (8 * rank_of(sq)); }
constexpr Bitboard file_bb(Square sq) { return kBbFileA << file_of(sq); }

// add a square to a bitboard / test whether a square is in it, e.g. bb | Square::E4, bb & Square::E4
constexpr Bitboard operator|(Bitboard bb, Square sq) { return bb | square_bb(sq); }
constexpr Bitboard operator&(Bitboard bb, Square sq) { return bb & square_bb(sq); }

/*
 * moves every set bit one step in dir; drops bits that would leave the board
 * (wrapping from the H file to the A file and back)
 */
constexpr Bitboard shift(Bitboard bb, Direction dir) {
  switch (dir) {
    case Direction::North:     return bb << 8;
    case Direction::South:     return bb >> 8;
    case Direction::East:      return (bb & ~kBbFileH) << 1;
    case Direction::West:      return (bb & ~kBbFileA) >> 1;
    case Direction::NorthEast: return (bb & ~kBbFileH) << 9;
    case Direction::NorthWest: return (bb & ~kBbFileA) << 7;
    case Direction::SouthEast: return (bb & ~kBbFileH) >> 7;
    case Direction::SouthWest: return (bb & ~kBbFileA) >> 9;
  }
  return kBbNone;
}

// number of set squares
constexpr int popcount(Bitboard bb) { return std::popcount(bb); }

// lowest set square (closest to A1)
constexpr Square lsb(Bitboard bb) {
  assert(bb != kBbNone);
  return Square(std::countr_zero(bb));
}

/*
 * removes the lowest set square from bb and returns it;
 *   while (bb) { Square sq = pop_lsb(bb); ... }
 */
constexpr Square pop_lsb(Bitboard& bb) {
  const Square sq = lsb(bb);
  bb &= bb - 1;
  return sq;
}

}  // namespace chess

#endif  // BITBOARD_HPP
