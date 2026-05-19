#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "transposition_table\transposition_table.hpp"
#include "move_ordering\move_ordering.hpp"
#include "..\evaluation\evaluation.hpp"
#include "include/chess.hpp"
#include "..\uci\uci.hpp"

#include <limits>
#include <array>
#include <atomic>
#include <chrono>

namespace Search {
    struct Limits {
        static constexpr int MATE_SCORE = 32000;
        static constexpr int INF        = 32767;
        static constexpr int START_PLY  = 0;
        static constexpr int MAX_PLY    = 32;
        static constexpr int SEARCH_CANCELLED  = -INF;
        static constexpr int REPEATED_POSITION = 0;
    };

    class Search {
        public:
            void search_position(UCI::Info info);
            
            void clear_tables() { tt_table.clear(); }

        private:
            TranspositionTable::TranspositionTable tt_table;
            std::atomic<bool> can_search {true};
            int nodes_searched = 0;
            std::chrono::high_resolution_clock::time_point search_start;

            int quiescence_search(chess::Board& board, int ply, int alpha, int beta);
            int negamax(chess::Board& board, int ply, int ply_remaining, int alpha, int beta);
    };
}

#endif // SEARCH_HPP