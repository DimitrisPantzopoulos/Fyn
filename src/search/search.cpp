#include "tables/transposition_table/transposition_table.hpp"
#include "tables/history_table/history_table.hpp"
#include "move_ordering/move_ordering.hpp"
#include "../evaluation/evaluation.hpp"
#include "misc/search_helper.hpp"
#include "../uci/uci.hpp"

#include "include/chess.hpp"
#include "search.hpp"
#include "misc/search_helper.hpp"

#include <thread>
#include <atomic>
#include <vector>

#define HANDLE_CANCEL_SEARCH(can_search)                             \
    do {                                                             \
        if (!can_search) { return Limits::SEARCH_CANCELLED;  }       \
    } while(0)                                                       \


#define HANDLE_REPETITION(board)                                               \
    do {                                                                       \
        if (board.isRepetition(1)) { return Limits::REPEATED_POSITION; }       \
    } while (0)                                                                \


#define HANDLE_EMPTY_LEGAL_MOVES(board, legal_moves, ply)         \
    do {                                                          \
        if ((legal_moves).empty()) {                              \
            if ((board).inCheck()) {                              \
                return -(Limits::MATE_SCORE - ply);               \
            } else {                                              \
                return 0;                                         \
            }                                                     \
        }                                                         \
    } while (0)

#define HANDLE_JOIN_TIMER_THREAD(timer_thread)                  \
    do {                                                        \
        if (timer_thread.joinable()) { timer_thread.join(); }   \
    } while (0)                                                 \

int Search::Search::quiescence_search(chess::Board& board, int ply, int alpha, int beta) {
    nodes_searched++;
    HANDLE_CANCEL_SEARCH(can_search);
    HANDLE_REPETITION(board);

    const chess::Movelist legal_moves = order_moves(board, chess::Move::NO_MOVE, ply, true);

    int stand_pat = Evaluation::evaluation(board);

    if (stand_pat >= beta) { return beta; }
    if (stand_pat > alpha) { alpha = stand_pat; }

    for (const chess::Move &move : legal_moves) {
        if (!SearchHelper::see(board, move, -100)) { continue; }
         
        board.makeMove(move);
        int eval = -quiescence_search(board, ply + 1, -beta, -alpha);
        board.unmakeMove(move);
        
        HANDLE_CANCEL_SEARCH(can_search);

        if (eval  > alpha) { alpha = eval; }
        if (alpha >= beta) { return beta;  }
    }

    return alpha;
}

int Search::Search::negamax(chess::Board& board, int ply, int depth, int alpha, int beta) {
    nodes_searched++;
    HANDLE_CANCEL_SEARCH(can_search);
    HANDLE_REPETITION(board);

    chess::Move hash_move    = chess::Move::NO_MOVE;
    uint64_t    hash         = board.hash();
    int         origin_alpha = alpha;

    TranspositionTable::TranspositionEntry tt_entry;

    if (tt_table.retrieve(hash, tt_entry)){
        hash_move = tt_entry.best_move;

        if (tt_entry.depth >= depth) {
            int tt_eval = tt_entry.eval;
            
            if      (tt_entry.type == TranspositionTable::TTNodeType::LOWERBOUND)             { alpha = std::max(alpha, tt_eval); }
            else if (tt_entry.type == TranspositionTable::TTNodeType::UPPERBOUND)             { beta  = std::min(beta, tt_eval);  }
            if      (tt_entry.type == TranspositionTable::TTNodeType::EXACT || alpha >= beta) { return tt_eval; }
        }
    }

    if (depth <= 0) { return quiescence_search(board, ply, alpha, beta); }
    
    const bool in_check = board.inCheck();
    const bool is_pv = beta - alpha > 1;

    // Reverse Futility Pruning
    if (depth <= 3 && !in_check && !is_pv) {
        const int static_eval = Evaluation::evaluation(board);
        int margin = Params::RFP_MARGIN * depth;

        if (static_eval - margin >= beta) {
            return static_eval - margin;
        }
    }

    // Null Move Pruning
    if (depth >= 3 && !is_pv && !in_check && beta < Limits::MATE_SCORE - Params::NMP_OFFSET && board.hasNonPawnMaterial(board.sideToMove())){
        board.makeNullMove();
        int null_move_score = -negamax(board, ply + 1, depth - 1 - (Params::NULL_MOVE_R + depth / 6), -beta, -beta+1);
        board.unmakeNullMove();

        if (null_move_score >= beta) { return beta; }
    }

    const chess::Movelist legal_moves = order_moves(board, hash_move, ply, false);
    
    HANDLE_EMPTY_LEGAL_MOVES(board, legal_moves, ply);

    // History Maluses collection
    std::vector<chess::Move> quiets_searched;

    for (int i=0; i<legal_moves.size(); i++) {
        const chess::Move move = legal_moves[i];
        int eval;

        const bool is_capture   = board.isCapture(move);
        const bool gives_check  = board.givesCheck(move) != chess::CheckType::NO_CHECK;
        const bool is_promotion = move.promotionType()   != chess::PieceType::NONE;

        const int extension = SearchHelper::calculate_extension(gives_check);
        const int new_depth = depth - 1 + extension;

        board.makeMove(move);
        if (i == 0) {
            eval = -negamax(board, ply + 1, new_depth, -beta, -alpha);
        } else {
            const int reduction = SearchHelper::calculate_reduction(
                i, depth, is_pv, in_check, is_capture, gives_check
            );

            const int reduced_depth = std::max(0, new_depth - reduction);

            eval = -negamax(board, ply + 1, reduced_depth, -(alpha + 1), -alpha);

            if (reduction > 0 && eval > alpha) {
                eval = -negamax(board, ply + 1, new_depth, -(alpha + 1), -alpha);
            }

            if (eval > alpha && eval < beta) {
                eval = -negamax(board, ply + 1, new_depth, -beta, -alpha);
            }
        }
        board.unmakeMove(move);
        
        HANDLE_CANCEL_SEARCH(can_search);

        if (eval  > alpha) { alpha = eval; hash_move = move; }

        if (alpha >= beta) {
            if (!is_capture && !is_promotion) {
                const int bonus = this->history_table.bonus_formula(depth);
                const chess::Color stm = board.sideToMove();

                history_table.update_history(stm, move, bonus);

                for (const chess::Move& quiet : quiets_searched) {
                    history_table.update_history(stm, quiet, -bonus);
                }

                km_table.update_killers(move, ply);
            }

            // Store tt table entry
            tt_table.store(hash, TranspositionTable::TTNodeType::LOWERBOUND, depth, beta, move);
            return beta;
        }

        // Collect quiets for history maluses.
        if (!is_capture && !is_promotion) {
            quiets_searched.push_back(move);
        }
    }

    tt_table.store(hash, TranspositionTable::get_node_type(origin_alpha, alpha, beta), depth, alpha, hash_move);
    return alpha;
}

void Search::Search::search_position(UCI::Info info) {
    // Launch Timer thread
    can_search = true;
    auto timer_thread = std::thread(SearchHelper::timer, info.milliseconds, std::ref(can_search));

    // Search Position
    chess::Move best_move   = chess::Move::NO_MOVE;
    int         best_eval   = -Limits::INF;
    int         depth_searched = 0;
    nodes_searched = 0;
    
    for (int depth = 1; depth <= info.depth; depth++) {
        if (!can_search) { break; }
        
        const chess::Movelist legal_moves = order_moves(info.board, best_move, Limits::START_PLY, false);
        
        // Reset
        best_eval = -Limits::INF;
        int alpha = -Limits::INF;
        int beta  = Limits::INF;

        for (const chess::Move &move : legal_moves) {
            info.board.makeMove(move);
            int eval = -negamax(info.board, Limits::START_PLY + 1, depth - 1, -beta, -alpha);
            info.board.unmakeMove(move);
            
            if (!can_search) { HANDLE_JOIN_TIMER_THREAD(timer_thread); break;}
        
            if (eval > best_eval) {
                best_eval   = eval;
                best_move   = move;
                depth_searched = depth;
            }

            if (eval > alpha) {
                alpha = eval;
            }
        }

        SearchHelper::print_search_info(best_eval, depth, nodes_searched, best_move, false);
        if (!can_search) { break; }
    }

    HANDLE_JOIN_TIMER_THREAD(timer_thread);
    SearchHelper::print_search_info(best_eval, depth_searched, nodes_searched, best_move, true);
}