#ifndef SEARCH_HELPER_HPP
#define SEARCH_HELPER_HPP

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

        std::cout << "info score cp " << best_eval << " depth " << depth << " nodes "  << nodes << "\n";

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
}

#endif