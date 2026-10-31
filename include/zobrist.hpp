#ifndef ZOBRIST_HPP
#define ZOBRIST_HPP

#include <cassert>
#include <cstdint>

#include "primitives.hpp"

namespace chess {

using Key = std::uint64_t;

namespace zobrist {
namespace detail {

constexpr int kPieces          = 1 + (kColors * kTypes);  // Piece::None included
constexpr int kCastlingSubsets = 16;

/*
 * 64-bit counter run through a bijective mixer, so no two keys can ever repeat.
 * changing seed changes every key, which only matters once keys are persisted.
 * polyglot opening books use their own fixed keys computed separately when probing
 */
constexpr Key splitmix64(Key& state) {
  Key z = (state += 0x9E3779B97F4A7C15ULL);
  z     = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
  z     = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
  return z ^ (z >> 31);
}

struct Keys {
  Key piece_square[kPieces][kSquares]{};
  Key castling[kCastlingSubsets]{};
  Key ep_file[8]{};
  Key side = 0;
};

constexpr Keys make_keys() {
  Key state = 0x5EED'C4E5'5000'0001ULL;
  Keys keys;
  for (int p = 1; p < kPieces; ++p) {
    for (int sq = 0; sq < kSquares; ++sq) keys.piece_square[p][sq] = splitmix64(state);
  }
  for (int cr = 1; cr < kCastlingSubsets; ++cr) keys.castling[cr] = splitmix64(state);
  for (Key& k : keys.ep_file) k = splitmix64(state);
  keys.side = splitmix64(state);
  return keys;
}

inline constexpr Keys kKeys = make_keys();

}  // namespace detail

// key for a piece standing on a square (zero for Piece::None)
constexpr Key piece_square(Piece p, Square sq) {
  assert(sq != Square::None);
  return detail::kKeys.piece_square[peel(p)][peel(sq)];
}

// key for a full set of castling rights (zero for none)
constexpr Key castling(CastlingRights rights) { return detail::kKeys.castling[peel(rights)]; }

// key for an en passant square by file
constexpr Key ep(Square sq) {
  assert(sq != Square::None);
  return detail::kKeys.ep_file[file_of(sq)];
}

// key toggled when black is to move
constexpr Key side() { return detail::kKeys.side; }

}  // namespace zobrist
}  // namespace chess

#endif  // ZOBRIST_HPP
