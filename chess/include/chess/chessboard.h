#include <array>
#include <memory>

#include "chesspiece.h"

namespace chess{
class chess_board {
  using chess_piece_ptr = std::unique_ptr<chess_piece>;
  using board_data = std::array<std::array<chess_piece_ptr, 8>, 8>;
  board_data m_pieces;

  auto get_piece_at(size_t const i, size_t const j) -> chess_piece_ptr&;
  auto get_piece_at(size_t const i, size_t const j) const -> chess_piece_ptr const&;
};
}
