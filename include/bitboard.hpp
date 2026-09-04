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
using Bitboard = std::uint64_t;

constexpr Bitboard BB_ALL  = ~0; // universal set
constexpr Bitboard BB_NONE = 0;  // empty set

constexpr Bitboard operator|(Bitboard bb, Square sq) { return bb | static_cast<uint64_t>(sq); }
constexpr Bitboard operator&(Bitboard bb, Square sq) { return bb & static_cast<uint64_t>(sq); }

constexpr Bitboard shift(Bitboard bb, Direction dir) { return bb << static_cast<uint64_t>(dir); }
constexpr Bitboard square_bb(Square sq) { return BB_NONE << static_cast<uint64_t>(sq); }


} // namespace chess

#endif // BITBOARD_HPP