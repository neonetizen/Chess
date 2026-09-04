/*
 * TODO:
 *   - add to_string for debug printing?
 *   - enum operator| overloads
 */
#ifndef BITBOARD_HPP
#define BITBOARD_HPP

#include <cstdint>

namespace chess {

    using Bitboard = std::uint64_t;
    constexpr Bitboard BB_ALL  = 0xFFFFFFFFFFFFFFFFULL;
    constexpr Bitboard BB_NONE = 0;

} // namespace chess

#endif // BITBOARD_HPP