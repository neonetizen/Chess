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
    Color side_to_move;             // drives movegen
    CastlingRights castling;        // 4-bit enum updated by hand-rules
    Square ep_square;               // calculated from previous ply; sq directly behind pawn who double-pushed
    int halfmove_clock;             // 50-move-rule counter
    int fullmove_number;            // current move number of the side to move

    constexpr Bitboard occupied() const {
        return occupancy[peel(Color::White)] | occupancy[peel(Color::Black)];
    }
};

} // namespace chess

#endif // POSITION_HPP