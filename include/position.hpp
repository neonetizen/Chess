/*
 * TODO:
 *   - position struct for:
 *     - create mailbox array Piece board[64]
 *     - bitboard per color, per type
 *     - occupancy bitboard by color
 *     - occupied() derived for both colors
 *     - side to move
 *     - castling rights
 *     - en passsant square
 *     - halfmove_clock
 *     - full move number
 */
#ifndef POSITION_HPP
#define POSITION_HPP

#include "bitboard.hpp"
#include "primitives.hpp"

constexpr auto SQUARES = 64;
constexpr auto COLORS = 2;
constexpr auto TYPES = 6;

namespace chess {
struct Position {
    Piece board[SQUARES];           // "redundancy" for checking piece locations without probing each array
    Bitboard pieces[COLORS][TYPES]; 
    Bitboard occupancy[COLORS];     // superset of all occupied squares of a color
    constexpr Bitboard occupied() const { return occupancy[idx(Color::White)] | occupancy[idx(Color::Black)]; }
};

} // namespace chess

#endif // POSITION_HPP