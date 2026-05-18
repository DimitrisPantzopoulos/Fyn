#ifndef MOVE_ORDERING
#define MOVE_ORDERING

#include "include/chess.hpp"

struct OrderingLimits {
    static const int MAX_MOVES = 256;
};

struct OrderingHeuristics {
    static const int PV_MOVE = 1000;
};

template <chess::movegen::MoveGenType Type>
inline chess::Movelist get_legal_moves(const chess::Board& board) {
    chess::Movelist legal_moves;
    chess::movegen::legalmoves<Type>(legal_moves, board);
    return legal_moves;
}

chess::Movelist order_moves(const chess::Board& board, const chess::Move& pv_move, bool in_qsearch);

#endif