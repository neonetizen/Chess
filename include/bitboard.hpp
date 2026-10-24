/*
 * TODO:
 *   - add to_string for debug printing?
 */
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

constexpr Bitboard BB_ALL  = ~Bitboard{0}; // universal set
constexpr Bitboard BB_NONE = 0;            // empty set

constexpr Bitboard BB_FILE_A = 0x0101010101010101ULL;
constexpr Bitboard BB_FILE_B = BB_FILE_A << 1;
constexpr Bitboard BB_FILE_C = BB_FILE_A << 2;
constexpr Bitboard BB_FILE_D = BB_FILE_A << 3;
constexpr Bitboard BB_FILE_E = BB_FILE_A << 4;
constexpr Bitboard BB_FILE_F = BB_FILE_A << 5;
constexpr Bitboard BB_FILE_G = BB_FILE_A << 6;
constexpr Bitboard BB_FILE_H = BB_FILE_A << 7;

constexpr Bitboard BB_RANK_1 = 0xFFULL;
constexpr Bitboard BB_RANK_2 = BB_RANK_1 << (8 * 1);
constexpr Bitboard BB_RANK_3 = BB_RANK_1 << (8 * 2);
constexpr Bitboard BB_RANK_4 = BB_RANK_1 << (8 * 3);
constexpr Bitboard BB_RANK_5 = BB_RANK_1 << (8 * 4);
constexpr Bitboard BB_RANK_6 = BB_RANK_1 << (8 * 5);
constexpr Bitboard BB_RANK_7 = BB_RANK_1 << (8 * 6);
constexpr Bitboard BB_RANK_8 = BB_RANK_1 << (8 * 7);

// precondition: sq != Square::None (shifting a 64-bit value by 64 is UB, [expr.shift]/1)
constexpr Bitboard square_bb(Square sq) {
    assert(sq != Square::None);
    return Bitboard{1} << peel(sq);
}

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
        case Direction::East:      return (bb & ~BB_FILE_H) << 1;
        case Direction::West:      return (bb & ~BB_FILE_A) >> 1;
        case Direction::NorthEast: return (bb & ~BB_FILE_H) << 9;
        case Direction::NorthWest: return (bb & ~BB_FILE_A) << 7;
        case Direction::SouthEast: return (bb & ~BB_FILE_H) >> 7;
        case Direction::SouthWest: return (bb & ~BB_FILE_A) >> 9;
    }
    return BB_NONE;
}

constexpr int popcount(Bitboard bb) { return std::popcount(bb); }

// precondition: bb != BB_NONE
constexpr Square lsb(Bitboard bb) {
    assert(bb != BB_NONE);
    return Square(std::countr_zero(bb));
}

// returns the lowest set square and clears it from bb; precondition: bb != BB_NONE
constexpr Square pop_lsb(Bitboard& bb) {
    const Square sq = lsb(bb);
    bb &= bb - 1;
    return sq;
}

} // namespace chess

#endif // BITBOARD_HPP
