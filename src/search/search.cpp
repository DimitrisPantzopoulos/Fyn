#include "transposition_table\transposition_table.hpp"
#include "move_ordering\move_ordering.hpp"
#include "..\evaluation\evaluation.hpp"
#include "..\uci\uci.hpp"

#include "include/chess.hpp"
#include "search.hpp"

#include <chrono>
#include <thread>
#include <atomic>

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

void timer(int milliseconds, std::atomic<bool>& can_search) {
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    can_search = false;
}

void print_search_info(int best_eval, int depth, int nodes, chess::Move best_move, bool print_best_move){
    const std::string best_move_uci = (best_move == chess::Move::NO_MOVE) ? "0000" : chess::uci::moveToUci(best_move);

    std::cout << "info score cp " << best_eval << " depth " << depth << " nodes "  << nodes << "\n";

    if (print_best_move)
        std::cout << "bestmove "      << best_move_uci << std::endl;
}

int Search::Search::quiescence_search(chess::Board& board, int ply, int alpha, int beta) {
    nodes_searched++;
    HANDLE_CANCEL_SEARCH(can_search);
    HANDLE_REPETITION(board);

    const chess::Movelist legal_moves = order_moves(board, chess::Move::NO_MOVE, true);

    int stand_pat = Evaluation::evaluation(board);

    if (stand_pat >= beta) { return beta; }
    if (stand_pat > alpha) { alpha = stand_pat; }

    for (const chess::Move &move : legal_moves) {
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
        if (tt_entry.depth >= depth) {
            int tt_eval = tt_entry.eval;
            hash_move   = tt_entry.best_move;
            
            if      (tt_entry.type == TranspositionTable::TTNodeType::LOWERBOUND)             { alpha = std::max(alpha, tt_eval); }
            else if (tt_entry.type == TranspositionTable::TTNodeType::UPPERBOUND)             { beta  = std::min(beta, tt_eval);  }
            if      (tt_entry.type == TranspositionTable::TTNodeType::EXACT || alpha >= beta) { return tt_eval; }
        }
    }

    if (depth <= 0) { return quiescence_search(board, ply, alpha, beta); }
    
    bool in_check = board.inCheck();
    bool is_pv = beta - alpha > 1;

    // Reverse Futility Pruning
    if (depth <= 3 && !in_check && !is_pv) {
        int static_eval = Evaluation::evaluation(board);
        int margin = Params::RFP_MARGIN * depth;

        if (static_eval - margin >= beta) {
            return static_eval - margin;
        }
    }

    // Null Move Pruning
    if (depth >= 3 && !is_pv && !in_check && beta < Limits::MATE_SCORE - 1000 && board.hasNonPawnMaterial(board.sideToMove())){
        board.makeNullMove();
        int null_move_score = -negamax(board, ply + 1, depth - 1 - (Params::NULL_MOVE_R + depth / 6), -beta, -beta+1);
        board.unmakeNullMove();

        if (null_move_score >= beta) { return beta; }
    }

    const chess::Movelist legal_moves = order_moves(board, hash_move, false);
    
    HANDLE_EMPTY_LEGAL_MOVES(board, legal_moves, ply);

    for (int i=0; i<legal_moves.size(); i++) {
        const chess::Move move = legal_moves[i];
        int eval;

        board.makeMove(move);
        // PVS Search
        if (i == 0) {
            eval = -negamax(board, ply + 1, depth - 1, -beta, -alpha);
        } else {
            eval = -negamax(board, ply + 1, depth - 1, -(alpha + 1), -alpha);

            if (eval > alpha && eval < beta) {
                eval = -negamax(board, ply + 1, depth - 1, -beta, -alpha);
            }
        }
        board.unmakeMove(move);
        
        HANDLE_CANCEL_SEARCH(can_search);

        if (eval  > alpha) { alpha = eval; hash_move = move; }

        if (alpha >= beta) { 
            tt_table.store(hash, TranspositionTable::TTNodeType::LOWERBOUND, depth, beta, move);
            return beta;
        }
    }

    tt_table.store(hash, TranspositionTable::get_node_type(origin_alpha, alpha, beta), depth, alpha, hash_move);
    return alpha;
}

void Search::Search::search_position(UCI::Info info) {
    // Launch Timer thread
    can_search = true;
    auto timer_thread = std::thread(timer, info.milliseconds, std::ref(can_search));

    // Search Position
    chess::Move best_move   = chess::Move::NO_MOVE;
    int         best_eval   = -Limits::INF;
    int         depth_searched = 0;
    nodes_searched = 0;
    
    for (int depth = 1; depth <= info.depth; depth++) {
        const chess::Movelist legal_moves = order_moves(info.board, best_move, false);
        
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
        }

        if (!can_search) { break; }

        print_search_info(best_eval, depth, nodes_searched, best_move, false);
    }

    HANDLE_JOIN_TIMER_THREAD(timer_thread);
    print_search_info(best_eval, depth_searched, nodes_searched, best_move, true);
}