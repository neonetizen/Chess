#include <format>
#include <iostream>
#include <string>

#include "debug.hpp"
#include "log.hpp"

namespace chess {
namespace {

template <typename SquareChar>
std::string grid(SquareChar square_char) {
  CHESS_TRACE_FUNC();
  std::string out;
  for (int rank = 7; rank >= 0; --rank) {
    out += static_cast<char>('1' + rank);
    out += ' ';
    for (int file = 0; file < 8; ++file) {
      out += ' ';
      out += square_char(make_square(file, rank));
    }
    out += '\n';
  }
  out += "\n   a b c d e f g h\n";
  return out;
}

}  // namespace

std::string to_string(Square sq) {
  CHESS_TRACE_FUNC();
  if (sq == Square::None) return "-";
  return {static_cast<char>('a' + file_of(sq)), static_cast<char>('1' + rank_of(sq))};
}

std::string to_string(CastlingRights rights) {
  CHESS_TRACE_FUNC();
  std::string out;
  if (any(rights & CastlingRights::WhiteOO)) out += 'K';
  if (any(rights & CastlingRights::WhiteOOO)) out += 'Q';
  if (any(rights & CastlingRights::BlackOO)) out += 'k';
  if (any(rights & CastlingRights::BlackOOO)) out += 'q';
  return out.empty() ? "-" : out;
}

std::string to_string(Bitboard bb) {
  CHESS_TRACE_FUNC();
  return grid([bb](Square sq) { return (bb & sq) != BB_NONE ? '1' : '.'; }) + std::format("   0x{:016x}\n", bb);
}

std::string to_string(const Position& pos) {
  CHESS_TRACE_FUNC();
  return grid([&pos](Square sq) { return piece_char(pos.board[peel(sq)]); }) +
         std::format("\nside: {}  castling: {}  ep: {}  halfmove: {}  fullmove: {}\n",
                     pos.side_to_move == Color::White ? 'w' : 'b', to_string(pos.castling), to_string(pos.ep_square),
                     pos.halfmove_clock, pos.fullmove_number);
}

void print(Bitboard bb) {
  CHESS_TRACE_FUNC();
  print(bb, std::cout);
}
void print(Bitboard bb, std::ostream& os) {
  CHESS_TRACE_FUNC();
  os << to_string(bb) << '\n';
}
void print(const Position& pos) {
  CHESS_TRACE_FUNC();
  print(pos, std::cout);
}
void print(const Position& pos, std::ostream& os) {
  CHESS_TRACE_FUNC();
  os << to_string(pos) << '\n';
}

}  // namespace chess
