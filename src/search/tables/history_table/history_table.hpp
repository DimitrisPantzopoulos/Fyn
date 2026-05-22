#ifndef HISTORY_HPP
#define HISTORY_HPP

#include "include/chess.hpp"

#include <algorithm>
#include <cstdint>

namespace HistoryTable {
    namespace Limits {
        static constexpr int MAX_HISTORY = 16'384;
        static constexpr int MIN_HISTORY = -MAX_HISTORY;
    }

    namespace Params {
        static constexpr int BASE_MULTIPLIER = 300;
        static constexpr int OFFSET = 250;
    }

    struct HistoryTable
    {
        int history[2][64][64];

        void clear() {
            std::memset(history, 0, sizeof(history));
        }

        int bonus_formula(const int depth){
            return Params::BASE_MULTIPLIER * depth - Params::OFFSET;
        }

        void update_history(const chess::Color stm, const chess::Move move, const int bonus) {
            const int clamped_bonus = std::clamp(bonus, Limits::MIN_HISTORY, Limits::MAX_HISTORY);
            const int color = stm == chess::Color::WHITE ? 0 : 1;
            const int from_index = move.from().index();
            const int to_index = move.to().index();
            
            int& value = history[color][from_index][to_index];

            value += clamped_bonus - value * std::abs(clamped_bonus) / Limits::MAX_HISTORY;
            value = std::clamp(value, Limits::MIN_HISTORY, Limits::MAX_HISTORY);
        }

        int get_history(const chess::Color stm, const chess::Move move) const {
            const int color = stm == chess::Color::WHITE ? 0 : 1;
            const int from_index = move.from().index();
            const int to_index = move.to().index();

            return history[color][from_index][to_index];
        }
    };
    
}
#endif