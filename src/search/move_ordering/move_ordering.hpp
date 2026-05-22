#ifndef MOVE_ORDERING
#define MOVE_ORDERING

#include "include/chess.hpp"

struct OrderingLimits {
    static constexpr int MAX_MOVES = 256;
};

struct OrderingHeuristics {
    static constexpr int PV_MOVE               = 1'000'000;
    static constexpr int PRIMARY_KILLER_MOVE   =   900'000;
    static constexpr int SECONDARY_KILLER_MOVE =   800'000;
};

template <chess::movegen::MoveGenType Type>
inline chess::Movelist get_legal_moves(const chess::Board& board) {
    chess::Movelist legal_moves;
    chess::movegen::legalmoves<Type>(legal_moves, board);
    return legal_moves;
}

#endif