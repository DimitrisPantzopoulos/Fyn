#include "include/chess.hpp"
#include "evaluation.hpp"

#include <algorithm>
#include <cstddef>

int Evaluation::evaluation(const chess::Board& board) {
    int color = board.sideToMove() == chess::Color::WHITE ? 1 : -1;

    Score total = {0, 0};

    // Ignore nullptr they are used to tune the evaluation function
    total  = Evaluation::evaluate_material(board, nullptr);
    total += Evaluation::evaluate_psts(board,     nullptr);
    total += Evaluation::pawn_evaluation(board,   nullptr);

    const int game_phase = calculate_game_phase(board);

    return color * calculate_tapered_eval(total, game_phase);
}

