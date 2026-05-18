#ifndef TRANSPOSITION_TABLE_HPP
#define TRANSPOSITION_TABLE_HPP

#include "include/chess.hpp"

#include <algorithm>
#include <vector>
#include <cstdint>
#include <cstddef>

enum TTNodeType {
    LOWERBOUND,
    EXACT,
    UPPERBOUND,
};

struct TranspositionEntry {
    uint64_t    hash;
    uint8_t     depth;
    int16_t     eval;
    TTNodeType  type;
    chess::Move best_move;
};

struct TranspositionTable {
    std::vector<TranspositionEntry> table;
    size_t size;

    TranspositionTable(size_t mb = 64) {
        resize_table(mb);
    }

    void resize_table(size_t mb) {
        size = (mb * 1024 * 1024) / sizeof(TranspositionEntry);
        table.assign(size, TranspositionEntry{});
    }

    void clear() {
        std::fill(table.begin(), table.end(), TranspositionEntry{});
    }

    inline TranspositionEntry& probe(uint64_t hash) {
        return table[hash % size];
    }

    inline void store(uint64_t hash, TTNodeType type, int depth, int eval, chess::Move best_move) {
        TranspositionEntry& entry = table[hash % size];

        // Replacement strategy: replace if deeper or empty
        if (entry.hash == 0 || entry.depth <= depth) {
            entry.hash  = hash;
            entry.eval  = static_cast<int16_t>(eval);
            entry.depth = static_cast<uint8_t>(depth);
            entry.type  = type;
            entry.best_move = best_move;
        }
    }

    inline bool retrieve(uint64_t hash, TranspositionEntry& out) const {
        const TranspositionEntry& entry = table[hash % size];
        if (entry.hash == hash) {
            out = entry;
            return true;
        }
        return false;
    }
};

inline TTNodeType get_node_type(int origin_alpha, int alpha, int beta) {
    if      (alpha <= origin_alpha) { return TTNodeType::UPPERBOUND; }
    else if (alpha >= beta)         { return TTNodeType::LOWERBOUND; }
    return TTNodeType::EXACT;
}

#endif