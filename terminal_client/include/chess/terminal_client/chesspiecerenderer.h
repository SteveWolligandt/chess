#pragma once
//==============================================================================
#include "chess/chesspiece.h"
#include "chess/networkinstance.h"
//==============================================================================
namespace chess::terminal_renderer {
class chess_piece_terminal_renderer : public network_instance {
  virtual char get_possible_moves(chess_board const& board) const = 0;
};
}
