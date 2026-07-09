#include "include/chess.hpp"
#include "evaluation.hpp"

#include <algorithm>
#include <cstddef>

int Evaluation::evaluation(const chess::Board& board) {
    int color = board.sideToMove() == chess::Color::WHITE ? 1 : -1;

    Score total = {0, 0};
    const uint8_t game_phase = calculate_game_phase(board);

    // Ignore nullptr they are used to tune the evaluation function
    total  = Evaluation::evaluate_material(board, nullptr);
    total += Evaluation::evaluate_psts(board,     nullptr);
    total += Evaluation::pawn_evaluation(board,   nullptr);

    return color * (total.mg * game_phase + total.eg * (Params::MAX_PHASE - game_phase)) / Params::MAX_PHASE;
}

