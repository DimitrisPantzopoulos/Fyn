#ifndef KMT_HPP
#define KMT_HPP

#include "include/chess.hpp"

#include <array>

namespace KillerMoveTable{
    static constexpr int MAX_KILLER_PLY = 32;

    struct KillerMovesTable {
        chess::Move killers[MAX_KILLER_PLY][2];
        
        void clear() {
            for (int ply = 0; ply < MAX_KILLER_PLY; ply++) {
                killers[ply][0] = chess::Move::NO_MOVE;
                killers[ply][1] = chess::Move::NO_MOVE;
            }
        }

        void update_killers(const chess::Move move, const int ply) {
            if (killers[ply][0] != move) {
                killers[ply][1] = killers[ply][0];
                killers[ply][0] = move;
            }
        }

        chess::Move get_primary_killer(const int ply) const {
            return killers[ply][0];
        }

        chess::Move get_secondary_killer(const int ply) const {
            return killers[ply][1];
        }
    };
}
#endif