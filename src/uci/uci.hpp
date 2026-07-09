#ifndef UCI_HPP
#define UCI_HPP

#include "include/chess.hpp"
#include <cstdint>

namespace UCI {
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
    
    struct Info {
        // Board position info
        chess::Board    board;
        chess::Movelist moves;

        // Time info
        uint32_t w_time = 0;
        uint32_t b_time = 0;

        uint32_t w_inc = 0;
        uint32_t b_inc = 0;
        
        // Search Info
        uint32_t milliseconds = 0;
        uint32_t depth = 256;
    };

    void uci_protocol();
}

#endif // UCI_HPP