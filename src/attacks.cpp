#include <cstdint>
#include <vector>

#include "attacks.hpp"
#include "log.hpp"

namespace chess {
namespace {

using detail::Magic;

// one slice per square, sized 2^(bits in that square's mask)
constexpr std::size_t kBishopTableSize = 5248;
constexpr std::size_t kRookTableSize   = 102400;

Bitboard bishop_table[kBishopTableSize];
Bitboard rook_table[kRookTableSize];

// xorshift64*: seeded with fixed numbers; magic search finds the same magics every run
struct Prng {
  std::uint64_t state;

  std::uint64_t next() {
    state ^= state >> 12;
    state ^= state << 25;
    state ^= state >> 27;
    return state * 2685821657736338717ULL;
  }

  // few set bits
  std::uint64_t sparse() { return next() & next() & next(); }
};

/*
 * squares whose occupancy can change the slider's attacks
 */
Bitboard relevant_mask(Square sq, Bitboard (*slow)(Square, Bitboard)) {
  const Bitboard edges = ((kBbRank1 | kBbRank8) & ~rank_bb(sq)) | ((kBbFileA | kBbFileH) & ~file_bb(sq));
  return slow(sq, kBbNone) & ~edges;
}

void init_slider(Magic (&magics)[kSquares], Bitboard* table, std::size_t table_size,
                 Bitboard (*slow)(Square, Bitboard)) {
  CHESS_TRACE_FUNC();
  constexpr std::size_t kMaxPatterns = 4096;  // 2^12; rook in corner
  std::vector<Bitboard> occupancy(kMaxPatterns);
  std::vector<Bitboard> reference(kMaxPatterns);
  std::vector<int> epoch(kMaxPatterns, 0);
  [[maybe_unused]] int attempt = 0;

  Bitboard* next = table;
  for (int s = 0; s < kSquares; ++s) {
    const auto sq = Square(s);
    Magic& m      = magics[s];
    m.mask        = relevant_mask(sq, slow);
    m.shift       = static_cast<unsigned>(64 - popcount(m.mask));
    m.table       = next;

    // carry-rippler trick: b = (b - mask) & mask steps through every subset of mask, ending back at 0
    std::size_t size = 0;
    Bitboard b       = kBbNone;
    do {
      occupancy[size] = b;
      reference[size] = slow(sq, b);
      ++size;
      b = (b - m.mask) & m.mask;
    } while (b != kBbNone);

#ifdef CHESS_PEXT
    for (std::size_t i = 0; i < size; ++i) next[m.index(occupancy[i])] = reference[i];
#else
    // per-rank seeds from Stockfish
    constexpr std::uint64_t kSeeds[8] = {728, 10316, 55013, 32803, 12281, 15100, 16645, 255};
    Prng rng{kSeeds[rank_of(sq)]};

    /*
     * keep trying random magics until every blocker pattern lands on a slot that is either unused or already holds
     * the same attacks
     * epoch marks which slots were written by the current attempt, so the slice doesn't need clearing between tries
     */
    for (std::size_t i = 0; i < size;) {
      do {
        m.magic = rng.sparse();
      } while (popcount((m.mask * m.magic) >> 56) < 6);  // cheap early rejections
      ++attempt;

      for (i = 0; i < size; ++i) {
        const std::size_t idx = m.index(occupancy[i]);
        if (epoch[idx] < attempt) {
          epoch[idx] = attempt;
          next[idx]  = reference[i];
        } else if (next[idx] != reference[i]) {
          break;
        }
      }
    }
#endif
    next += size;
  }
  assert(next == table + table_size);
  CHESS_DEBUG("slider tables filled, {} magic attempts", attempt);
}

}  // namespace

void init_attacks() {
  CHESS_TRACE_FUNC();
  init_slider(detail::bishop_magics, bishop_table, kBishopTableSize, detail::bishop_attacks_slow);
  init_slider(detail::rook_magics, rook_table, kRookTableSize, detail::rook_attacks_slow);
}

}  // namespace chess
