#pragma once

#include "networkinstance.h"

namespace chess {
class chess_board;

class chess_piece : public network_instance {
  virtual char get_possible_moves(chess_board const& board) const = 0;
};
}
