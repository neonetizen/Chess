#pragma once

#include <set>

enum Color {
    White,
    Black
};

enum Name: unsigned char {
    Pawn = 'p',
    Knight = 'n',
    Bishop = 'b',
    Rook = 'r',
    Queen = 'q',
    King = 'k'
};

struct Piece {
    // constructor
    Piece(const Color color, const Name name);
    // deconstructor
    ~Piece() = default;

    // trivial getter for moves
    const std::set<std::pair<int, int>> getMoves() const;
    // trivial getter for name
    const Name getName() const { return name; }

    void print() const;
private:
    const Color color;
    const Name name;
    const std::set<std::pair<int, int>> moves;
};