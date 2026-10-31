#include <algorithm>
#include <array>
#include <charconv>
#include <cstdint>
#include <limits>
#include <optional>
#include <string>
#include <string_view>

#include <spdlog/fmt/fmt.h>

#include "debug.hpp"
#include "fen.hpp"
#include "log.hpp"

namespace chess {
namespace {

constexpr size_t kMaxFields = 6;

struct Fields {
  std::array<std::string_view, kMaxFields> field{};
  size_t count = 0;
};

std::optional<Fields> split_fields(std::string_view fen) {
  CHESS_TRACE_FUNC();
  Fields out;
  for (size_t i = 0; i < fen.size();) {
    if (fen[i] == ' ') {
      ++i;
      continue;
    }
    if (out.count == kMaxFields) {
      CHESS_ERROR("bad fen '{}': more than {} fields", fen, kMaxFields);
      return std::nullopt;
    }
    const size_t end       = std::min(fen.find(' ', i), fen.size());
    out.field[out.count++] = fen.substr(i, end - i);
    i                      = end;
  }
  if (out.count < 4) {
    CHESS_ERROR("bad fen '{}': expected at least 4 fields, got {}", fen, out.count);
    return std::nullopt;
  }
  return out;
}

/*
 * FEN lists ranks from 8 down to 1, separated by '/', and each rank from the a file to the h file.
 * see FEN wiki if help needed to read
 */
bool parse_placement(std::string_view fen, std::string_view placement, Position& pos) {
  CHESS_TRACE_FUNC();
  int rank = 7;
  int file = 0;
  for (const char c : placement) {
    if (c == '/') {
      if (file != 8) {
        CHESS_ERROR("bad fen '{}': rank {} has {} squares", fen, rank + 1, file);
        return false;
      }
      if (rank == 0) {
        CHESS_ERROR("bad fen '{}': more than 8 ranks", fen);
        return false;
      }
      --rank;
      file = 0;
      continue;
    }

    const bool is_digit = c >= '1' && c <= '8';
    const Piece p       = is_digit ? Piece::None : piece_from_char(c);
    if (!is_digit && p == Piece::None) {
      CHESS_ERROR("bad fen '{}': unexpected '{}' in piece placement", fen, c);
      return false;
    }
    const int width = is_digit ? c - '0' : 1;
    if (file + width > 8) {
      CHESS_ERROR("bad fen '{}': rank {} has more than 8 squares", fen, rank + 1);
      return false;
    }
    if (!is_digit) pos.put_piece(p, make_square(file, rank));
    file += width;
  }
  if (rank != 0 || file != 8) {
    CHESS_ERROR("bad fen '{}': piece placement must be 8 ranks of 8 squares", fen);
    return false;
  }
  return true;
}

std::optional<CastlingRights> parse_castling(std::string_view fen, std::string_view field) {
  CHESS_TRACE_FUNC();
  CastlingRights rights = CastlingRights::None;
  if (field == "-") return rights;
  for (const char c : field) {
    CastlingRights right = CastlingRights::None;
    switch (c) {
      case 'K': right = CastlingRights::WhiteOO; break;
      case 'Q': right = CastlingRights::WhiteOOO; break;
      case 'k': right = CastlingRights::BlackOO; break;
      case 'q': right = CastlingRights::BlackOOO; break;
      default:  CHESS_ERROR("bad fen '{}': unexpected '{}' in castling rights", fen, c); return std::nullopt;
    }
    if (any(rights & right)) {
      CHESS_ERROR("bad fen '{}': castling right '{}' repeated", fen, c);
      return std::nullopt;
    }
    rights |= right;
  }
  return rights;
}

std::optional<Square> parse_square(std::string_view s) {
  CHESS_TRACE_FUNC();
  if (s.size() != 2 || s[0] < 'a' || s[0] > 'h' || s[1] < '1' || s[1] > '8') return std::nullopt;
  return make_square(s[0] - 'a', s[1] - '1');
}

std::optional<int> parse_count(std::string_view s, int max) {
  CHESS_TRACE_FUNC();
  int value            = 0;
  const auto [ptr, ec] = std::from_chars(s.data(), s.data() + s.size(), value);
  if (ec != std::errc{} || ptr != s.data() + s.size() || value < 0 || value > max) return std::nullopt;
  return value;
}

}  // namespace

std::optional<Position> Position::from_fen(std::string_view fen) {
  CHESS_TRACE_FUNC();
  const auto fields = split_fields(fen);
  if (!fields) return std::nullopt;
  const auto& f = fields->field;

  Position pos;
  if (!parse_placement(fen, f[0], pos)) return std::nullopt;

  if (f[1] != "w" && f[1] != "b") {
    CHESS_ERROR("bad fen '{}': side to move must be 'w' or 'b', got '{}'", fen, f[1]);
    return std::nullopt;
  }
  pos.side_to_move_ = f[1] == "w" ? Color::White : Color::Black;

  const auto castling = parse_castling(fen, f[2]);
  if (!castling) return std::nullopt;
  pos.castling_ = *castling;

  if (f[3] != "-") {
    const auto ep = parse_square(f[3]);
    if (!ep) {
      CHESS_ERROR("bad fen '{}': en passant square must be '-' or a square, got '{}'", fen, f[3]);
      return std::nullopt;
    }
    /*
     * many FEN writers set the ep square after every double push. keeping a dead one gives the
     * same position two different keys; only keep it if a pawn can actually take.
     * TODO: drop it when every ep capture is illegal (pinned capturer), once movegen can tell
     */
    if (ep_capturable(pos, *ep)) {
      pos.ep_square_ = *ep;
    } else {
      CHESS_DEBUG("fen '{}': dropping en passant square {}, no pawn can capture there", fen, f[3]);
    }
  }

  constexpr int kMaxHalfmove = std::numeric_limits<decltype(pos.halfmove_clock_)>::max();
  constexpr int kMaxFullmove = std::numeric_limits<decltype(pos.fullmove_number_)>::max();
  const auto halfmove        = fields->count > 4 ? parse_count(f[4], kMaxHalfmove) : 0;
  if (!halfmove) {
    CHESS_ERROR("bad fen '{}': halfmove clock must be a number from 0 to {}, got '{}'", fen, kMaxHalfmove, f[4]);
    return std::nullopt;
  }
  const auto fullmove = fields->count > 5 ? parse_count(f[5], kMaxFullmove) : 1;
  if (!fullmove) {
    CHESS_ERROR("bad fen '{}': fullmove number must be a number from 0 to {}, got '{}'", fen, kMaxFullmove, f[5]);
    return std::nullopt;
  }
  pos.halfmove_clock_  = static_cast<uint8_t>(*halfmove);
  pos.fullmove_number_ = static_cast<uint16_t>(*fullmove);

  pos.key_      = compute_key(pos);
  pos.pawn_key_ = compute_pawn_key(pos);
  return pos;
}

std::string to_fen(const Position& pos) {
  CHESS_TRACE_FUNC();
  std::string placement;
  for (int rank = 7; rank >= 0; --rank) {
    int empty = 0;
    for (int file = 0; file < 8; ++file) {
      const Piece p = pos.piece_on(make_square(file, rank));
      if (p == Piece::None) {
        ++empty;
        continue;
      }
      if (empty > 0) placement += static_cast<char>('0' + empty);
      empty = 0;
      placement += piece_char(p);
    }
    if (empty > 0) placement += static_cast<char>('0' + empty);
    if (rank > 0) placement += '/';
  }

  return fmt::format("{} {} {} {} {} {}", placement, pos.side_to_move() == Color::White ? 'w' : 'b',
                     to_string(pos.castling()), to_string(pos.ep_square()), pos.halfmove_clock(),
                     pos.fullmove_number());
}

}  // namespace chess
