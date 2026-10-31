#ifndef FEN_HPP
#define FEN_HPP

#include <string>
#include <string_view>

#include "position.hpp"

namespace chess {

// default starting position
inline constexpr std::string_view kStartposFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// writes a Position back out as a FEN string
std::string to_fen(const Position& pos);

}  // namespace chess

#endif  // FEN_HPP
