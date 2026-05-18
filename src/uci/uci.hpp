#ifndef UCI_HPP
#define UCI_HPP

#include "include/chess.hpp"
#include <cstdint>

namespace UCI {
    struct Info {
        // Board position info
        chess::Board    board;
        chess::Movelist moves;

        // Time info
        uint16_t w_time;
        uint16_t b_time;

        uint16_t w_inc;
        uint16_t b_inc;
        
        // Search Info
        uint16_t milliseconds;
        uint16_t depth = 256;
    };

    enum Command : uint8_t {
        GO,
        UCI,
        STOP,
        QUIT,
        ISREADY,
        POSITION,
        UCINEWGAME,
        NONE,
    };

    void uci_protocol();
}

#endif // UCI_HPP