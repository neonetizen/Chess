/*
 * TODO:
 *   - add to_string for debug printing?
 */
#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP

#include <cassert>
#include <cstdint>
#include <type_traits>

namespace chess {

inline constexpr uint8_t SQUARES = 64;
inline constexpr uint8_t COLORS = 2;
inline constexpr uint8_t TYPES = 6;

// clang-format off
enum struct Square : uint8_t {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    None, // sentinel (e.g. no en passant square)
};
// clang-format on

enum struct PieceType : uint8_t {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
};

// clang-format off
enum struct Piece : uint8_t {
    None,
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK,
};
// clang-format on

enum struct Color : uint8_t {
    White,
    Black,
};

/*
 * values are the square-index offset of one step in that direction
 */
enum struct Direction : int8_t {
    // (A1 = 0, B1 = 1, A2 = 8)
    North     =  8,
    NorthEast =  9,
    East      =  1,
    SouthEast = -7,
    South     = -8,
    SouthWest = -9,
    West      = -1,
    NorthWest =  7,
};

/*
 * clearing a right: castling &= ~CastlingRights::WhiteOO
 * querying rights:  any(castling & (right | rights...))
 */
enum struct CastlingRights : uint8_t {
    None      = 0b0000,
    WhiteOO   = 0b0001,
    WhiteOOO  = 0b0010,
    BlackOO   = 0b0100,
    BlackOOO  = 0b1000,

    All = WhiteOO | WhiteOOO | BlackOO | BlackOOO,
};

/*
 * function to peel back enums to their integral types
 */
template<typename T>
    requires std::is_enum_v<T>
constexpr auto peel(T e) -> std::underlying_type_t<T> {
    return static_cast<std::underlying_type_t<T>>(e);
}

constexpr CastlingRights operator|(CastlingRights a, CastlingRights b) {
    return CastlingRights(peel(a) | peel(b));
}
constexpr CastlingRights operator&(CastlingRights a, CastlingRights b) {
    return CastlingRights(peel(a) & peel(b));
}
// masked so that unused high bits arent set
constexpr CastlingRights operator~(CastlingRights a) {
    return CastlingRights(~peel(a) & peel(CastlingRights::All));
}
constexpr CastlingRights& operator|=(CastlingRights& a, CastlingRights b) { return a = a | b; }
constexpr CastlingRights& operator&=(CastlingRights& a, CastlingRights b) { return a = a & b; }

constexpr bool any(CastlingRights rights) { return rights != CastlingRights::None; }

constexpr int file_of(Square sq) { return peel(sq) & 7; }  // 0 = A file
constexpr int rank_of(Square sq) { return peel(sq) >> 3; } // 0 = 1st rank

constexpr Square make_square(int file, int rank) {
    assert(file >= 0 && file < 8 && rank >= 0 && rank < 8);
    return Square((rank * 8) + file);
}

constexpr Piece make_piece(Color c, PieceType pt) {
    return Piece(1 + (peel(c) * TYPES) + peel(pt));
}

// precondition for both: p != Piece::None
constexpr PieceType type_of(Piece p) {
    assert(p != Piece::None);
    return PieceType((peel(p) - 1) % TYPES);
}
constexpr Color color_of(Piece p) {
    assert(p != Piece::None);
    return Color((peel(p) - 1) / TYPES);
}

constexpr Color operator~(Color c) { return Color(peel(c) ^ 1); }

} // namespace chess

#endif // PRIMITIVES_HPP
