#ifndef POSITION_HPP
#define POSITION_HPP

#include <cassert>
#include <cstdint>
#include <optional>
#include <string_view>
#include <type_traits>

#include "bitboard.hpp"
#include "primitives.hpp"

namespace chess {

class alignas(64) Position {
 public:
  // reads FEN string into a Position, or nullopt if it isn't a valid FEN
  static std::optional<Position> from_fen(std::string_view fen);

  // piece on a square, or Piece::None if it's empty
  constexpr Piece piece_on(Square sq) const { return board_[peel(sq)]; }

  // pieces of one type (both colors), of one color, or of one color and type
  constexpr Bitboard pieces(PieceType pt) const { return by_type_[peel(pt)]; }
  constexpr Bitboard pieces(Color c) const { return by_color_[peel(c)]; }
  constexpr Bitboard pieces(Color c, PieceType pt) const { return by_color_[peel(c)] & by_type_[peel(pt)]; }

  // all occupied squares
  constexpr Bitboard occupied() const { return by_type_[kAll]; }

  constexpr Color side_to_move() const { return side_to_move_; }
  constexpr CastlingRights castling() const { return castling_; }
  constexpr Square ep_square() const { return ep_square_; }
  constexpr int halfmove_clock() const { return halfmove_clock_; }
  constexpr int fullmove_number() const { return fullmove_number_; }

  // places a piece on an empty square
  constexpr void put_piece(Piece p, Square sq) {
    assert(p != Piece::None && board_[peel(sq)] == Piece::None);
    const Bitboard bb = square_bb(sq);
    board_[peel(sq)]  = p;
    by_type_[peel(type_of(p))] |= bb;
    by_type_[kAll] |= bb;
    by_color_[peel(color_of(p))] |= bb;
  }

  // takes a piece off an occupied square
  constexpr void remove_piece(Square sq) {
    const Piece p = board_[peel(sq)];
    assert(p != Piece::None);
    const Bitboard bb = square_bb(sq);
    board_[peel(sq)]  = Piece::None;
    by_type_[peel(type_of(p))] &= ~bb;
    by_type_[kAll] &= ~bb;
    by_color_[peel(color_of(p))] &= ~bb;
  }

  bool operator==(const Position&) const = default;

 private:
  static constexpr int kAll = kTypes;  // by_type_ slot holding every piece

  Bitboard by_type_[kTypes + 1]{};
  Bitboard by_color_[kColors]{};
  Piece board_[kSquares]{};                  // "redundancy" for checking piece locations without probing each array
  Color side_to_move_       = Color::White;  // drives movegen
  CastlingRights castling_  = CastlingRights::None;  // 4-bit enum updated by hand-rules
  Square ep_square_         = Square::None;  // calculated from previous ply; sq directly behind pawn who double-pushed
  uint8_t halfmove_clock_   = 0;             // 50-move-rule counter
  uint16_t fullmove_number_ = 1;             // current move number of the side to move
};

// copied per ply so it must stay plain bytes
static_assert(std::is_trivially_copyable_v<Position> && std::is_standard_layout_v<Position>);

// true if any piece of color by attacks sq
bool is_attacked(const Position& pos, Square sq, Color by);

// true if the position is legal chess and its board[]/bitboards agree
bool is_valid(const Position& pos);

}  // namespace chess

#endif  // POSITION_HPP
