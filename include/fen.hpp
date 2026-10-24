#ifndef FEN_HPP
#define FEN_HPP

#include <optional>
#include <string>
#include <string_view>

#include "position.hpp"

namespace chess {

// default starting position
inline constexpr std::string_view STARTPOS_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

// reads FEN string into a Position, or nullopt if it isn't a valid FEN
std::optional<Position> from_fen(std::string_view fen);

// writes a Position back out as a FEN string
std::string to_fen(const Position& pos);

}  // namespace chess

#endif  // FEN_HPP
