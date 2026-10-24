#ifndef DEBUG_HPP
#define DEBUG_HPP

#include <iosfwd>
#include <string>

#include "bitboard.hpp"
#include "position.hpp"
#include "primitives.hpp"

namespace chess {

std::string to_string(Square sq);              // "e4", "-" for Square::None
std::string to_string(CastlingRights rights);  // "KQkq" subset, "-" for none
std::string to_string(Bitboard bb);            // 1/. grid, rank 8 on top, plus hex value
std::string to_string(const Position& pos);    // piece grid (FEN letters) plus state line

void print(Bitboard bb);
void print(Bitboard bb, std::ostream& os);
void print(const Position& pos);
void print(const Position& pos, std::ostream& os);

}  // namespace chess

#endif  // DEBUG_HPP
