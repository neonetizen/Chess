#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iosfwd>
#include <string>

#include "bitboard.hpp"
#include "position.hpp"
#include "primitives.hpp"

namespace chess {

std::string to_string(Square sq);              // e.g. "e4", or "-" for Square::None
std::string to_string(CastlingRights rights);  // e.g. "KQkq", or "-" for none
std::string to_string(Bitboard bb);            // 8x8 grid of 1s and .s, plus the hex value
std::string to_string(const Position& pos);    // 8x8 grid of pieces, plus side/castling/ep/move counters

void print(Bitboard bb);
void print(Bitboard bb, std::ostream& os);
void print(const Position& pos);
void print(const Position& pos, std::ostream& os);

}  // namespace chess

#endif  // DEBUG_HPP
