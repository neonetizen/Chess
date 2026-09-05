/*
 * TODO:
 *   - add to_string for debug printing?
 *   - enum operator| overloads
 */
#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include <cstdint>

#include "primitives.hpp"

namespace chess {
/*
 * 0 0 0 0 0 0 0 0   8
 * 0 0 0 0 0 0 0 0   7 
 * 0 0 0 0 0 0 0 0   6
 * 0 0 0 0 0 0 0 0   5
 * 0 0 0 0 0 0 0 0   4
 * 0 0 0 0 0 0 0 0   3
 * 0 0 0 0 0 0 0 0   2
 * 0 0 0 0 0 0 0 0   1
 *
 * H G F E D C B A
 */

 // :)
using Bitboard = std::uint64_t;

constexpr Bitboard BB_ALL  = ~0; // universal set
constexpr Bitboard BB_NONE = 0;  // empty set

constexpr Bitboard operator|(Bitboard bb, Square sq) { return bb | peel(sq); }
constexpr Bitboard operator&(Bitboard bb, Square sq) { return bb & peel(sq); }

constexpr Bitboard shift(Bitboard bb, Direction dir) { return bb << peel(dir); }
constexpr Bitboard square_bb(Square sq) { return Bitboard{1} << peel(sq); }


} // namespace chess

#endif // BITBOARD_HPP