#include "attacks.hpp"
#include "debug.hpp"
#include "log.hpp"
#include "position.hpp"

namespace chess {
namespace {

bool castling_ok(const Position& pos, CastlingRights right, Square king_sq, Piece king, Square rook_sq, Piece rook) {
  CHESS_TRACE_FUNC();
  if (!any(pos.castling() & right)) return true;
  return pos.piece_on(king_sq) == king && pos.piece_on(rook_sq) == rook;
}

bool bitboards_match_board(const Position& pos) {
  CHESS_TRACE_FUNC();
  Bitboard all = kBbNone;
  for (int c = 0; c < kColors; ++c) {
    Bitboard color_bb = kBbNone;
    for (int t = 0; t < kTypes; ++t) {
      const Piece p       = make_piece(Color(c), PieceType(t));
      Bitboard from_board = kBbNone;
      for (int sq = 0; sq < kSquares; ++sq) {
        if (pos.piece_on(Square(sq)) == p) from_board |= square_bb(Square(sq));
      }
      if (pos.pieces(Color(c), PieceType(t)) != from_board) {
        CHESS_ERROR("invalid position: '{}' bitboard doesn't match board[]", piece_char(p));
        return false;
      }
      color_bb |= from_board;
    }
    if (pos.pieces(Color(c)) != color_bb) {
      CHESS_ERROR("invalid position: {} occupancy doesn't match board[]", c == 0 ? "white" : "black");
      return false;
    }
    all |= color_bb;
  }
  Bitboard types = kBbNone;
  for (int t = 0; t < kTypes; ++t) types |= pos.pieces(PieceType(t));
  if (pos.occupied() != all || types != all) {
    CHESS_ERROR("invalid position: occupied squares don't match board[]");
    return false;
  }
  return true;
}

}  // namespace

bool is_attacked(const Position& pos, Square sq, Color by) {
  const Bitboard occupied = pos.occupied();
  const Bitboard queens   = pos.pieces(PieceType::Queen);
  // symmetric attacks
  return (pawn_attacks(~by, sq) & pos.pieces(by, PieceType::Pawn)) != kBbNone ||
         (knight_attacks(sq) & pos.pieces(by, PieceType::Knight)) != kBbNone ||
         (king_attacks(sq) & pos.pieces(by, PieceType::King)) != kBbNone ||
         (bishop_attacks(sq, occupied) & (pos.pieces(PieceType::Bishop) | queens) & pos.pieces(by)) != kBbNone ||
         (rook_attacks(sq, occupied) & (pos.pieces(PieceType::Rook) | queens) & pos.pieces(by)) != kBbNone;
}

bool is_valid(const Position& pos) {
  CHESS_TRACE_FUNC();
  if (!bitboards_match_board(pos)) return false;

  for (int c = 0; c < kColors; ++c) {
    const int kings = popcount(pos.pieces(Color(c), PieceType::King));
    if (kings != 1) {
      CHESS_ERROR("invalid position: {} has {} kings", c == 0 ? "white" : "black", kings);
      return false;
    }
  }

  if ((pos.pieces(PieceType::Pawn) & (kBbRank1 | kBbRank8)) != kBbNone) {
    CHESS_ERROR("invalid position: pawn on the first or eighth rank");
    return false;
  }

  if (!castling_ok(pos, CastlingRights::WhiteOO, Square::E1, Piece::WK, Square::H1, Piece::WR) ||
      !castling_ok(pos, CastlingRights::WhiteOOO, Square::E1, Piece::WK, Square::A1, Piece::WR) ||
      !castling_ok(pos, CastlingRights::BlackOO, Square::E8, Piece::BK, Square::H8, Piece::BR) ||
      !castling_ok(pos, CastlingRights::BlackOOO, Square::E8, Piece::BK, Square::A8, Piece::BR)) {
    CHESS_ERROR("invalid position: castling rights {} don't match king/rook placement", to_string(pos.castling()));
    return false;
  }

  if (pos.ep_square() != Square::None) {
    /*
     * if white is to move, black just pushed a pawn two squares
     * the ep square is the one it skipped, so the pawn sits one below it, and the square it came from is now empty
     */
    const bool white_to_move = pos.side_to_move() == Color::White;
    const int ep_rank        = white_to_move ? 5 : 2;
    const int ep             = peel(pos.ep_square());
    const int pushed_pawn_sq = white_to_move ? ep - 8 : ep + 8;
    const int push_origin_sq = white_to_move ? ep + 8 : ep - 8;
    const Piece pawn         = white_to_move ? Piece::BP : Piece::WP;
    if (rank_of(pos.ep_square()) != ep_rank || pos.piece_on(Square(pushed_pawn_sq)) != pawn ||
        pos.piece_on(Square(ep)) != Piece::None || pos.piece_on(Square(push_origin_sq)) != Piece::None) {
      CHESS_ERROR("invalid position: en passant square {} doesn't follow a double pawn push",
                  to_string(pos.ep_square()));
      return false;
    }
  }

  const Color waiting = ~pos.side_to_move();
  if (is_attacked(pos, lsb(pos.pieces(waiting, PieceType::King)), pos.side_to_move())) {
    CHESS_ERROR("invalid position: {} is in check but it's {}'s move", waiting == Color::White ? "white" : "black",
                waiting == Color::White ? "black" : "white");
    return false;
  }

  if (pos.fullmove_number() < 1) {
    CHESS_ERROR("invalid position: fullmove number must be at least 1, got {}", pos.fullmove_number());
    return false;
  }

  return true;
}

}  // namespace chess
