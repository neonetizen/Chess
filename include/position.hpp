#ifndef POSITION_HPP
#define POSITION_HPP

#include <cassert>

#include "bitboard.hpp"
#include "primitives.hpp"

namespace chess {
struct Position {
  Piece board[SQUARES]{};  // "redundancy" for checking piece locations without probing each array
  Bitboard pieces[COLORS][TYPES]{};
  Bitboard occupancy[COLORS]{};                    // superset of all occupied squares of a color
  Color side_to_move      = Color::White;          // drives movegen
  CastlingRights castling = CastlingRights::None;  // 4-bit enum updated by hand-rules
  Square ep_square        = Square::None;  // calculated from previous ply; sq directly behind pawn who double-pushed
  int halfmove_clock      = 0;             // 50-move-rule counter
  int fullmove_number     = 1;             // current move number of the side to move

  // all occupied squares
  constexpr Bitboard occupied() const { return occupancy[peel(Color::White)] | occupancy[peel(Color::Black)]; }

  bool operator==(const Position&) const = default;
};

// places a piece on an empty square
constexpr void put_piece(Position& pos, Piece p, Square sq) {
  assert(p != Piece::None && pos.board[peel(sq)] == Piece::None);
  pos.board[peel(sq)] = p;
  pos.pieces[peel(color_of(p))][peel(type_of(p))] |= square_bb(sq);
  pos.occupancy[peel(color_of(p))] |= square_bb(sq);
}

// takes a piece off an occupied square
constexpr void remove_piece(Position& pos, Square sq) {
  const Piece p = pos.board[peel(sq)];
  assert(p != Piece::None);
  pos.board[peel(sq)] = Piece::None;
  pos.pieces[peel(color_of(p))][peel(type_of(p))] &= ~square_bb(sq);
  pos.occupancy[peel(color_of(p))] &= ~square_bb(sq);
}

// true if any piece of color by attacks sq
bool is_attacked(const Position& pos, Square sq, Color by);

// true if the position is legal chess and its board[]/bitboards agree
bool is_valid(const Position& pos);

}  // namespace chess

#endif  // POSITION_HPP
