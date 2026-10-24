#include <format>
#include <iostream>
#include <string>

#include "debug.hpp"

namespace chess {
namespace {

constexpr char piece_char(Piece p) { return ".PNBRQKpnbrqk"[peel(p)]; }

// 8x8 grid, rank 8 first, with rank labels on the left and file labels below
template <typename SquareChar>
std::string grid(SquareChar square_char) {
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
  if (sq == Square::None) return "-";
  return {static_cast<char>('a' + file_of(sq)), static_cast<char>('1' + rank_of(sq))};
}

std::string to_string(CastlingRights rights) {
  std::string out;
  if (any(rights & CastlingRights::WhiteOO)) out += 'K';
  if (any(rights & CastlingRights::WhiteOOO)) out += 'Q';
  if (any(rights & CastlingRights::BlackOO)) out += 'k';
  if (any(rights & CastlingRights::BlackOOO)) out += 'q';
  return out.empty() ? "-" : out;
}

std::string to_string(Bitboard bb) {
  return grid([bb](Square sq) { return (bb & sq) != BB_NONE ? '1' : '.'; }) + std::format("   0x{:016x}\n", bb);
}

std::string to_string(const Position& pos) {
  return grid([&pos](Square sq) { return piece_char(pos.board[peel(sq)]); }) +
         std::format("\nside: {}  castling: {}  ep: {}  halfmove: {}  fullmove: {}\n",
                     pos.side_to_move == Color::White ? 'w' : 'b', to_string(pos.castling), to_string(pos.ep_square),
                     pos.halfmove_clock, pos.fullmove_number);
}

void print(Bitboard bb) { print(bb, std::cout); }
void print(Bitboard bb, std::ostream& os) { os << to_string(bb) << '\n'; }
void print(const Position& pos) { print(pos, std::cout); }
void print(const Position& pos, std::ostream& os) { os << to_string(pos) << '\n'; }

}  // namespace chess
