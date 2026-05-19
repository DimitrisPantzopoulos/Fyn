#include "include/chess.hpp"
#include "evaluation.hpp"

#include <algorithm>
#include <cstddef>

int Evaluation::evaluation(const chess::Board& board) {
    int color = board.sideToMove() == chess::Color::WHITE ? 1 : -1;

    Score white_score = {0, 0};
    Score black_score = {0, 0};

    uint8_t game_phase = 0;

    for (size_t type_idx = 0; type_idx < Params::NO_PIECE_TYPES; type_idx++) {
        const chess::PieceType type    = Params::piece_types[type_idx];
        
        chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
        chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);

        const Score   piece_value = Params::piece_type_value[type_idx];
        const uint8_t phase_value = Params::piece_type_phase[type_idx];

        const int white_pieces = white_bb.count();
        const int black_pieces = black_bb.count();

        white_score += piece_value * white_pieces;
        black_score += piece_value * black_pieces;

        while (white_bb) {white_score += Params::piece_type_pst[type_idx][white_bb.pop()];}
        while (black_bb) {black_score += Params::piece_type_pst[type_idx][black_bb.pop() ^ 56];}

        game_phase += phase_value * (white_pieces + black_pieces);
    }

    game_phase = std::min(game_phase, Params::MAX_PHASE);

    Score total = white_score - black_score;

    // total += Evaluation::pawn_evaluation(board);

    return color * (total.mg * game_phase + total.eg * (Params::MAX_PHASE - game_phase)) / Params::MAX_PHASE;
}
