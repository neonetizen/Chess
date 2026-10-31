#ifndef ATTACKS_HPP
#define ATTACKS_HPP

#include <array>
#include <cstddef>

#ifdef CHESS_PEXT
#include <immintrin.h>
#endif

#include "bitboard.hpp"
#include "primitives.hpp"

namespace chess {
namespace detail {

using AttackTable = std::array<Bitboard, kSquares>;

// a knight jump is one straight step plus one diagonal step away from it, so shift() handles the edge wrapping
constexpr Bitboard knight_jumps(Bitboard bb) {
  using enum Direction;
  return shift(shift(bb, North), NorthEast) | shift(shift(bb, North), NorthWest) | shift(shift(bb, South), SouthEast) |
         shift(shift(bb, South), SouthWest) | shift(shift(bb, East), NorthEast) | shift(shift(bb, East), SouthEast) |
         shift(shift(bb, West), NorthWest) | shift(shift(bb, West), SouthWest);
}

constexpr Bitboard king_steps(Bitboard bb) {
  using enum Direction;
  return shift(bb, North) | shift(bb, NorthEast) | shift(bb, East) | shift(bb, SouthEast) | shift(bb, South) |
         shift(bb, SouthWest) | shift(bb, West) | shift(bb, NorthWest);
}

template <typename F>
constexpr AttackTable make_table(F step) {
  AttackTable table{};
  for (std::size_t sq = 0; sq < kSquares; ++sq) table[sq] = step(square_bb(Square(sq)));
  return table;
}

inline constexpr AttackTable kKnightAttacks                    = make_table(knight_jumps);
inline constexpr AttackTable kKingAttacks                      = make_table(king_steps);
inline constexpr std::array<AttackTable, kColors> kPawnAttacks = {
    make_table([](Bitboard bb) { return shift(bb, Direction::NorthEast) | shift(bb, Direction::NorthWest); }),
    make_table([](Bitboard bb) { return shift(bb, Direction::SouthEast) | shift(bb, Direction::SouthWest); }),
};

// squares reached walking from sq in dir, stopping at (and including) the first occupied square
constexpr Bitboard slide(Square sq, Bitboard occupied, Direction dir) {
  Bitboard attacks = kBbNone;
  Bitboard bb      = square_bb(sq);
  while ((bb = shift(bb, dir)) != kBbNone) {
    attacks |= bb;
    if ((bb & occupied) != kBbNone) break;
  }
  return attacks;
}

// slow ray-walking slider attacks; used to fill the lookup tables & test
constexpr Bitboard bishop_attacks_slow(Square sq, Bitboard occupied) {
  using enum Direction;
  return slide(sq, occupied, NorthEast) | slide(sq, occupied, NorthWest) | slide(sq, occupied, SouthEast) |
         slide(sq, occupied, SouthWest);
}
constexpr Bitboard rook_attacks_slow(Square sq, Bitboard occupied) {
  using enum Direction;
  return slide(sq, occupied, North) | slide(sq, occupied, South) | slide(sq, occupied, East) |
         slide(sq, occupied, West);
}

/*
 * per-square lookup for one slider
 *   magic: multiply by a number that happens to give a unique index for every blocker pattern, keep the top bits
 *   pext:  the BMI2 instruction packs the masked bits together directly
 * reference chess programming wiki if needed
 */
struct Magic {
  Bitboard mask         = kBbNone;
  Bitboard magic        = 0;
  const Bitboard* table = nullptr;
  unsigned shift        = 0;

  std::size_t index(Bitboard occupied) const {
#ifdef CHESS_PEXT
    return _pext_u64(occupied, mask);
#else
    return ((occupied & mask) * magic) >> shift;
#endif
  }
};

inline Magic bishop_magics[kSquares];
inline Magic rook_magics[kSquares];

}  // namespace detail

// fills the bishop/rook lookup tables; call once at startup before any attack lookups
void init_attacks();

// squares a knight on sq attacks
constexpr Bitboard knight_attacks(Square sq) {
  assert(sq != Square::None);
  return detail::kKnightAttacks[peel(sq)];
}

// squares a king on sq attacks
constexpr Bitboard king_attacks(Square sq) {
  assert(sq != Square::None);
  return detail::kKingAttacks[peel(sq)];
}

// squares a pawn of color c on sq attacks (its two forward diagonals)
constexpr Bitboard pawn_attacks(Color c, Square sq) {
  assert(sq != Square::None);
  return detail::kPawnAttacks[peel(c)][peel(sq)];
}

// squares a bishop / rook / queen on sq attacks, given every occupied square on the board
inline Bitboard bishop_attacks(Square sq, Bitboard occupied) {
  assert(sq != Square::None);
  const detail::Magic& m = detail::bishop_magics[peel(sq)];
  assert(m.table != nullptr);  // init_attacks() not called
  return m.table[m.index(occupied)];
}
inline Bitboard rook_attacks(Square sq, Bitboard occupied) {
  assert(sq != Square::None);
  const detail::Magic& m = detail::rook_magics[peel(sq)];
  assert(m.table != nullptr);  // init_attacks() not called
  return m.table[m.index(occupied)];
}
inline Bitboard queen_attacks(Square sq, Bitboard occupied) {
  return bishop_attacks(sq, occupied) | rook_attacks(sq, occupied);
}

}  // namespace chess

#endif  // ATTACKS_HPP
