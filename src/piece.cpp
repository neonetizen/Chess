#include <cstdint>
#include <iostream>
#include <utility>

#include "includes/piece.hpp"

// TODO: remove magic numbers; allow board to change distances (allow for bigger than 8x8 boards)
// sets possible moves
static const std::set<std::pair<int, int>> setMoves(Color color, Name name) {
    std::set<std::pair<int, int>> moves;
    switch (name) {
        case Name::Pawn: {
            int_fast8_t dir = color == Color::White ? 1 : -1;

            moves.emplace(std::make_pair(0,  + (2 * dir)));
            moves.emplace(std::make_pair(0, dir));
            moves.emplace(std::make_pair(1, dir));
            moves.emplace(std::make_pair(-1, dir));
            
            return moves;
        }
        case Name::Knight: {
            int twos[2]{2, -2};
            int ones[2]{1, -1};

            for (auto doubl : twos) {
                for (auto singl : ones) {
                    moves.emplace(std::make_pair(doubl, singl));
                    moves.emplace(std::make_pair(singl, doubl));
                }
            }
            return moves;
        }
        case Name::Bishop: {
            for (auto i = 1; i <= 7; ++i) {
                        moves.emplace(std::make_pair(i, i));
                        moves.emplace(std::make_pair( i, -i));
                        moves.emplace(std::make_pair( -i, i));
                        moves.emplace(std::make_pair(-i, -i));
                }
            return moves;
        }
        case Name::Rook: {
            for (auto i = 1; i <= 7; ++i) {
                moves.emplace(std::make_pair(i, 0));
                moves.emplace(std::make_pair(-i, 0));
                moves.emplace(std::make_pair(0, i));
                moves.emplace(std::make_pair(0, -i));
            }
            return moves;
        }
        case Name::Queen: {
            for (auto i = 1; i <= 7; ++i) {
                moves.emplace(std::make_pair(i, i));
                moves.emplace(std::make_pair( i, -i));
                moves.emplace(std::make_pair( -i, i));
                moves.emplace(std::make_pair(-i, -i));

                moves.emplace(std::make_pair(i, 0));
                moves.emplace(std::make_pair(-i, 0));
                moves.emplace(std::make_pair(0, i));
                moves.emplace(std::make_pair(0, -i));
            }
            return moves;
        }
        case Name::King: {
            for (auto x = -1; x < 2; ++x) {
                for (auto y = -1; y < 2; ++y) {
                    if (x == 0 && y == 0)
                        continue;
                    moves.emplace(std::make_pair(x, y));
                }
            }
            return moves;
        }
    }
    return moves;
}

Piece::Piece(const Color color, const Name name) : color(color), name(name), moves(setMoves(color, name)) {}

void Piece::print() const {
    std::cout << name << std::endl;
}

const std::set<std::pair<int, int>> Piece::getMoves() const {
    return moves;
}