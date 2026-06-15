#ifndef SEARCH_HPP
#define SEARCH_HPP

#include "tables/transposition_table/transposition_table.hpp"
#include "tables/killer_move_table/killer_move_table.hpp"
#include "tables/history_table/history_table.hpp"
#include "move_ordering/move_ordering.hpp"
#include "../evaluation/evaluation.hpp"
#include "include/chess.hpp"
#include "../uci/uci.hpp"

#include <limits>
#include <array>
#include <atomic>

namespace Search {
    namespace Limits {
        static constexpr int MATE_SCORE = 32000;
        static constexpr int INF        = 32767;
        static constexpr int START_PLY  = 0;
        static constexpr int MAX_PLY    = 32;
        static constexpr int SEARCH_CANCELLED  = -INF;
        static constexpr int REPEATED_POSITION = 0;
    };

    namespace Params {
        static constexpr int RFP_MARGIN = 100;
        static constexpr int NULL_MOVE_R = 2;
        static constexpr int LMR_REDUCTION = 1;
        static constexpr int NMP_OFFSET = 1000;
    }

    class Search {
        public:
            void search_position(UCI::Info info);
            
            void clear_tables() { tt_table.clear(); km_table.clear(); history_table.clear(); }

        private:
            TranspositionTable::TranspositionTable tt_table;
            KillerMoveTable::KillerMovesTable km_table;
            HistoryTable::HistoryTable history_table;

            std::atomic<bool> can_search {true};
            int nodes_searched = 0;

            chess::Movelist order_moves(const chess::Board& board, const chess::Move& pv_move, const int ply, bool in_qsearch);
            int score_move(const chess::Board& board, const chess::Move& move, const chess::Move& pv_move, const int ply);
            
            int quiescence_search(chess::Board& board, int ply, int alpha, int beta);
            int negamax(chess::Board& board, int ply, int ply_remaining, int alpha, int beta);
    };
}

#endif // SEARCH_HPP