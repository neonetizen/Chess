#include "attacks.hpp"
#include "debug.hpp"
#include "log.hpp"
#include "position.hpp"

namespace chess {
namespace {

bool castling_ok(const Position& pos, CastlingRights right, Square king_sq, Piece king, Square rook_sq, Piece rook) {
  CHESS_TRACE_FUNC();
  if (!any(pos.castling & right)) return true;
  return pos.board[peel(king_sq)] == king && pos.board[peel(rook_sq)] == rook;
}

bool bitboards_match_board(const Position& pos) {
  CHESS_TRACE_FUNC();
  for (int c = 0; c < COLORS; ++c) {
    Bitboard color_bb = BB_NONE;
    for (int t = 0; t < TYPES; ++t) {
      const Piece p       = make_piece(Color(c), PieceType(t));
      Bitboard from_board = BB_NONE;
      for (int sq = 0; sq < SQUARES; ++sq) {
        if (pos.board[sq] == p) from_board |= square_bb(Square(sq));
      }
      if (pos.pieces[c][t] != from_board) {
        CHESS_ERROR("invalid position: '{}' bitboard doesn't match board[]", piece_char(p));
        return false;
      }
      color_bb |= from_board;
    }
    if (pos.occupancy[c] != color_bb) {
      CHESS_ERROR("invalid position: {} occupancy doesn't match board[]", c == 0 ? "white" : "black");
      return false;
    }
  }
  return true;
}

}  // namespace

bool is_attacked(const Position& pos, Square sq, Color by) {
  const auto& p           = pos.pieces[peel(by)];
  const Bitboard occupied = pos.occupied();
  const Bitboard queens   = p[peel(PieceType::Queen)];
  // symmetric attacks
  return (pawn_attacks(~by, sq) & p[peel(PieceType::Pawn)]) != BB_NONE ||
         (knight_attacks(sq) & p[peel(PieceType::Knight)]) != BB_NONE ||
         (king_attacks(sq) & p[peel(PieceType::King)]) != BB_NONE ||
         (bishop_attacks(sq, occupied) & (p[peel(PieceType::Bishop)] | queens)) != BB_NONE ||
         (rook_attacks(sq, occupied) & (p[peel(PieceType::Rook)] | queens)) != BB_NONE;
}

bool is_valid(const Position& pos) {
  CHESS_TRACE_FUNC();
  if (!bitboards_match_board(pos)) return false;

  for (int c = 0; c < COLORS; ++c) {
    const int kings = popcount(pos.pieces[c][peel(PieceType::King)]);
    if (kings != 1) {
      CHESS_ERROR("invalid position: {} has {} kings", c == 0 ? "white" : "black", kings);
      return false;
    }
  }

  const Bitboard pawns =
      pos.pieces[peel(Color::White)][peel(PieceType::Pawn)] | pos.pieces[peel(Color::Black)][peel(PieceType::Pawn)];
  if ((pawns & (BB_RANK_1 | BB_RANK_8)) != BB_NONE) {
    CHESS_ERROR("invalid position: pawn on the first or eighth rank");
    return false;
  }

  if (!castling_ok(pos, CastlingRights::WhiteOO, Square::E1, Piece::WK, Square::H1, Piece::WR) ||
      !castling_ok(pos, CastlingRights::WhiteOOO, Square::E1, Piece::WK, Square::A1, Piece::WR) ||
      !castling_ok(pos, CastlingRights::BlackOO, Square::E8, Piece::BK, Square::H8, Piece::BR) ||
      !castling_ok(pos, CastlingRights::BlackOOO, Square::E8, Piece::BK, Square::A8, Piece::BR)) {
    CHESS_ERROR("invalid position: castling rights {} don't match king/rook placement", to_string(pos.castling));
    return false;
  }

  if (pos.ep_square != Square::None) {
    /*
     * if white is to move, black just pushed a pawn two squares
     * the ep square is the one it skipped, so the pawn sits one below it, and the square it came from is now empty
     */
    const bool white_to_move = pos.side_to_move == Color::White;
    const int ep_rank        = white_to_move ? 5 : 2;
    const int ep             = peel(pos.ep_square);
    const int pushed_pawn_sq = white_to_move ? ep - 8 : ep + 8;
    const int push_origin_sq = white_to_move ? ep + 8 : ep - 8;
    const Piece pawn         = white_to_move ? Piece::BP : Piece::WP;
    if (rank_of(pos.ep_square) != ep_rank || pos.board[pushed_pawn_sq] != pawn || pos.board[ep] != Piece::None ||
        pos.board[push_origin_sq] != Piece::None) {
      CHESS_ERROR("invalid position: en passant square {} doesn't follow a double pawn push", to_string(pos.ep_square));
      return false;
    }
  }

  const Color waiting = ~pos.side_to_move;
  if (is_attacked(pos, lsb(pos.pieces[peel(waiting)][peel(PieceType::King)]), pos.side_to_move)) {
    CHESS_ERROR("invalid position: {} is in check but it's {}'s move", waiting == Color::White ? "white" : "black",
                waiting == Color::White ? "black" : "white");
    return false;
  }

  if (pos.halfmove_clock < 0 || pos.fullmove_number < 1) {
    CHESS_ERROR("invalid position: move counters out of range (halfmove {}, fullmove {})", pos.halfmove_clock,
                pos.fullmove_number);
    return false;
  }

  return true;
}

}  // namespace chess
