#include "chessboard.h"
namespace chess{
auto chess_board::get_piece_at(size_t const i, size_t const j) -> chess_piece_ptr & {
  return m_pieces[i][j];
}
auto chess_board::get_piece_at(size_t const i, size_t const j) const -> chess_piece_ptr const& {
  return m_pieces[i][j];
}
}
