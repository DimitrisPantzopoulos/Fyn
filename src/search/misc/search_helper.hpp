#ifndef SEARCH_HELPER_HPP
#define SEARCH_HELPER_HPP

#include "../evaluation/evaluation.hpp"
#include "../search.hpp"

#include <atomic>
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

namespace SearchHelper {
    inline void timer(int milliseconds, std::atomic<bool>& can_search) {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
        can_search = false;
    }

    inline void print_search_info(int best_eval, int depth, int nodes, chess::Move best_move, bool print_best_move) {
        const std::string best_move_uci = (best_move == chess::Move::NO_MOVE) ? "0000" : chess::uci::moveToUci(best_move);

        if (best_eval != -Search::Limits::INF) {
            std::cout << "info score cp " << best_eval << " depth " << depth << " nodes "  << nodes << "\n";
        }

        if (print_best_move)
            std::cout << "bestmove " << best_move_uci << std::endl;
    }

    inline int calculate_reduction(const int move_index, const int depth, const bool is_pv, const bool in_check, const bool is_capture, const bool gives_check) {
        if (
            move_index < 4  ||
            depth < 3       ||
            is_pv           ||
            in_check        ||
            is_capture      ||
            gives_check
        ) {
            return 0;
        }

        return Search::Params::LMR_REDUCTION;
    }

    inline int calculate_extension(const bool gives_check) {
        if (!gives_check) {return 0;}

        return 1;
    }

    inline bool see(const chess::Board& board, const chess::Move& move, const int threshold) {
        const chess::Square to   = move.to();
        const chess::Square from = move.from();

        const chess::Piece moving_piece = board.at<chess::Piece>(from);
        const chess::Color moving_side = moving_piece.color();

        const bool is_enpassant = move.typeOf() == chess::Move::ENPASSANT;
        
        const chess::Square captured_square = is_enpassant ? to.ep_square() : to;
        const chess::PieceType target = is_enpassant ? chess::PieceType::PAWN : board.at<chess::PieceType>(to);

        int value = Evaluation::piece_value(target) - threshold;

        if (value < 0) { return false; }

        const chess::PieceType attacker = moving_piece.type();

        value -= Evaluation::piece_value(attacker);

        if (value >= 0) { return true; }

        chess::Bitboard occ = (board.all() ^ (1ULL << from.index())) | (1ULL << to.index());

        if (is_enpassant) {
            occ ^= (1ULL << captured_square.index());
        }

        chess::Bitboard attackers = chess::attacks::attackers(board, chess::Color::WHITE, to) | 
                                    chess::attacks::attackers(board, chess::Color::BLACK, to);
        
        const chess::Bitboard queens  = board.pieces(chess::PieceType::QUEEN);
        const chess::Bitboard bishops = board.pieces(chess::PieceType::BISHOP);
        const chess::Bitboard rooks   = board.pieces(chess::PieceType::ROOK);

        if (attacker == chess::PieceType::PAWN || attacker == chess::PieceType::BISHOP || attacker == chess::PieceType::QUEEN) {
            attackers |= chess::attacks::bishop(to, occ) & (bishops | queens);
        }

        if (attacker == chess::PieceType::ROOK || attacker == chess::PieceType::QUEEN) {
            attackers |= chess::attacks::rook(to, occ) & (rooks | queens);
        }
        
        chess::Color stm = ~moving_side;

        while(true) {
            attackers &= occ;

            chess::Bitboard own_attackers = attackers & board.us(stm);

            if (!own_attackers) { break; }

            chess::PieceType piece_type;

            for (int pt = 0; pt <=5; pt++) {
                piece_type = chess::PieceType(static_cast<chess::PieceType::underlying>(pt));

                if (own_attackers & board.pieces(piece_type)) { 
                    break;
                };
            }

            stm = ~stm;

            if ((value = -value - 1 - Evaluation::piece_value(piece_type)) >= 0) {
                if (piece_type == chess::PieceType::KING && (attackers & board.us(stm))) {
                    stm = ~stm;
                }

                break;
            }

            occ ^= (1ULL << (own_attackers & board.pieces(piece_type)).lsb());

            if (piece_type == chess::PieceType::PAWN || piece_type == chess::PieceType::BISHOP || piece_type == chess::PieceType::QUEEN) {
                attackers |= chess::attacks::bishop(to, occ) & (bishops | queens);
            }

            if (piece_type == chess::PieceType::ROOK || piece_type == chess::PieceType::QUEEN) {
                attackers |= chess::attacks::rook(to, occ) & (rooks | queens);
            }
        }

        return stm != moving_side;
    }
}

#endif