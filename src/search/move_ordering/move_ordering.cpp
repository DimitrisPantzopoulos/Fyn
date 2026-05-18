#include "..\evaluation\evaluation.hpp"
#include "include/chess.hpp"
#include "move_ordering.hpp"

#include <algorithm>
#include <array>

struct ScoredMove {
    chess::Move move;
    int score;
};

int score_move(const chess::Board& board, const chess::Move& move, const chess::Move& pv_move) {
    if (move == pv_move)
        return OrderingHeuristics::PV_MOVE;

    if (board.at(move.to()) != chess::Piece::NONE) {
        int victim   = Evaluation::piece_value(board.at<chess::PieceType>(move.to()));
        int attacker = Evaluation::piece_value(board.at<chess::PieceType>(move.from()));

        return 10 * victim - attacker;
    }

    return 0;
}

chess::Movelist order_moves(const chess::Board& board, const chess::Move& pv_move, bool in_qsearch) {
    chess::Movelist moves = in_qsearch
        ? get_legal_moves<chess::movegen::MoveGenType::CAPTURE>(board)
        : get_legal_moves<chess::movegen::MoveGenType::ALL>(board);

    std::array<ScoredMove, OrderingLimits::MAX_MOVES> scored;
    int count = 0;

    for (int i = 0; i < moves.size(); ++i) {
        scored[count++] = ScoredMove{
            moves[i],
            score_move(board, moves[i], pv_move)
        };
    }

    std::sort(scored.begin(), scored.begin() + count,
        [](const ScoredMove& a, const ScoredMove& b) {
            return a.score > b.score;
        });

    chess::Movelist ordered;

    for (int i = 0; i < count; ++i) { ordered.add(scored[i].move); }

    return ordered;
}