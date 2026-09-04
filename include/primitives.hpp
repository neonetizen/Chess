/*
 * TODO:
 *   - add to_string for debug printing?
 *   - enum operator| overloads
 */
#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include <cstdint>

namespace chess {

enum struct Square : uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8
};

enum struct PieceType : uint8_t {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

enum struct Piece : uint8_t {
    None,
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK
};

enum struct Color : uint8_t {
    White,
    Black
};

[[nodiscard]] constexpr int idx(Color c) { return static_cast<int>(c); }

enum struct Direction : uint8_t {
    North,
    NorthEast,
    East,
    SouthEast,
    South,
    SouthWest,
    West,
    NorthWest
};

} // namespace chess

#endif // PRIMITIVES_HPP