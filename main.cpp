#include <iostream>

#include "src/includes/piece.hpp"

void print_moves(Piece piece) {
    std::cout << piece.getName() << ": ";
    for (auto move : piece.getMoves()) {
        std::cout << "(" << move.first << ", " << move.second << ") ";
    }
    std::cout << std::endl;
}

int main() {
    Piece pawn(Color::White, Name::Pawn);
    Piece pawn2(Color::Black, Name::Pawn);
    Piece knight(Color::White, Name::Knight);
    Piece bishop(Color::White, Name::Bishop);
    Piece rook(Color::White, Name::Rook);
    Piece queen(Color::White, Name::Queen);
    Piece king(Color::White, Name::King);

    print_moves(pawn);
    print_moves(pawn2);
    print_moves(knight);
    print_moves(bishop);
    print_moves(rook);
    print_moves(queen);
    print_moves(king);
}