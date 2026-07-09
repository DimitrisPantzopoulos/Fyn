#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include "include/chess.hpp"
#include <cstdint>

namespace Evaluation{
    struct Score {
        int mg;
        int eg;

        constexpr Score operator-(const Score& other) const {
            return {mg - other.mg, eg - other.eg};
        }

        constexpr Score& operator-=(const Score& other) {
            mg -= other.mg;
            eg -= other.eg;
            return *this;
        }

        constexpr Score operator+(const Score& other) const {
            return {mg + other.mg, eg + other.eg};
        }

        constexpr Score& operator+=(const Score& other) {
            mg += other.mg;
            eg += other.eg;
            return *this;
        }

        constexpr Score operator*(int k) const {
            return {mg * k, eg * k};
        }

        friend constexpr Score operator*(int k, const Score& score) {
            return score * k;
        }

        constexpr Score& operator*=(int k) {
            mg *= k;
            eg *= k;
            return *this;
        }

        constexpr Score operator/(int k) const {
            return {mg / k, eg / k};
        }

        constexpr Score& operator/=(int k) {
            mg /= k;
            eg /= k;
            return *this;
        }
    };

    struct EvalTrace {
        static constexpr int WHITE = 0;
        static constexpr int BLACK = 1;

        int passed_pawn[2][8] = {};
        int pst[2][6][64]     = {};
        int material[2][6]    = {};
    };

    namespace Params {
        static constexpr uint8_t NO_PIECE_TYPES = 6;
        static constexpr uint8_t MAX_PHASE      = 24;

        static constexpr chess::PieceType piece_types[NO_PIECE_TYPES] = {
            chess::PieceType::PAWN,
            chess::PieceType::KNIGHT,
            chess::PieceType::BISHOP,
            chess::PieceType::ROOK,
            chess::PieceType::QUEEN,
            chess::PieceType::KING,
        };
        
        static constexpr int8_t piece_type_phase[NO_PIECE_TYPES] = {
            0,  // PAWN
            1,  // KNIGHT
            1,  // BISHOP
            2,  // ROOK
            4,  // QUEEN
            0   // KING
        };

        static constexpr Score piece_type_pst[NO_PIECE_TYPES][64] = {
            { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {-8, 92}, {19, 90}, {-3, 87}, {1, 81}, {-1, 105}, {69, 104}, {129, 83}, {44, 17}, {1, 74}, {29, 88}, {22, 56}, {22, 57}, {51, 83}, {34, 74}, {108, 79}, {53, 35}, {2, 96}, {47, 101}, {35, 46}, {48, 46}, {63, 47}, {48, 61}, {85, 80}, {32, 53}, {12, 134}, {63, 122}, {51, 78}, {84, 18}, {110, 37}, {112, 48}, {92, 100}, {26, 82}, {11, 219}, {96, 182}, {123, 108}, {115, 21}, {148, 12}, {260, 59}, {131, 117}, {60, 124}, {130, 202}, {138, 190}, {136, 126}, {184, 25}, {169, 12}, {83, 55}, {-227, 187}, {-222, 194}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
            { {-235, -113}, {-11, -101}, {-97, 39}, {-40, 63}, {-14, 51}, {2, 20}, {0, -58}, {-208, -73}, {-62, 11}, {-25, 75}, {3, 75}, {30, 110}, {17, 121}, {28, 64}, {-10, 66}, {12, 13}, {-6, -16}, {31, 110}, {44, 139}, {60, 197}, {73, 187}, {51, 137}, {52, 111}, {16, -12}, {25, 75}, {44, 176}, {74, 235}, {63, 256}, {80, 251}, {70, 250}, {112, 185}, {55, 106}, {57, 82}, {84, 185}, {125, 232}, {144, 273}, {108, 280}, {165, 257}, {102, 226}, {122, 117}, {-10, 65}, {94, 154}, {159, 213}, {185, 220}, {270, 190}, {292, 219}, {179, 163}, {156, 52}, {33, 4}, {22, 92}, {120, 125}, {156, 205}, {172, 185}, {266, 73}, {11, 94}, {95, 8}, {-330, -167}, {-148, 60}, {-199, 179}, {62, 105}, {194, 93}, {-146, 182}, {-14, 63}, {-220, -212} },
            { {53, 43}, {50, 88}, {1, 31}, {-36, 98}, {-40, 107}, {2, 67}, {10, 86}, {35, 70}, {49, 66}, {58, 94}, {66, 112}, {25, 134}, {28, 132}, {44, 91}, {89, 105}, {70, -14}, {36, 109}, {53, 153}, {47, 186}, {62, 184}, {51, 185}, {50, 164}, {56, 122}, {72, 110}, {23, 120}, {64, 177}, {62, 208}, {81, 221}, {108, 208}, {54, 204}, {60, 166}, {60, 115}, {16, 150}, {85, 196}, {98, 192}, {171, 209}, {116, 243}, {147, 192}, {80, 224}, {59, 156}, {40, 140}, {72, 191}, {157, 180}, {137, 192}, {169, 184}, {196, 223}, {168, 188}, {105, 162}, {-36, 133}, {69, 183}, {57, 177}, {23, 196}, {63, 194}, {116, 168}, {42, 180}, {44, 113}, {-10, 155}, {-64, 188}, {-200, 196}, {-124, 212}, {-116, 184}, {-127, 193}, {136, 130}, {-22, 167} },
            { {-55, 292}, {-43, 319}, {-33, 341}, {-27, 335}, {-36, 332}, {-40, 327}, {28, 297}, {-36, 245}, {-152, 306}, {-66, 290}, {-59, 313}, {-70, 314}, {-71, 309}, {-33, 288}, {-1, 276}, {-142, 328}, {-97, 299}, {-71, 338}, {-83, 342}, {-71, 338}, {-74, 343}, {-81, 335}, {25, 304}, {-42, 285}, {-74, 352}, {-76, 396}, {-65, 407}, {-43, 395}, {-50, 383}, {-47, 384}, {3, 365}, {0, 333}, {-30, 392}, {6, 404}, {33, 414}, {80, 402}, {49, 405}, {66, 389}, {108, 353}, {74, 358}, {-5, 419}, {81, 399}, {88, 423}, {143, 394}, {191, 372}, {246, 381}, {317, 309}, {139, 367}, {69, 428}, {51, 449}, {121, 444}, {176, 445}, {158, 449}, {275, 361}, {146, 389}, {228, 361}, {177, 405}, {173, 423}, {133, 445}, {170, 429}, {194, 427}, {298, 405}, {301, 397}, {318, 389} },
            { {185, 501}, {161, 485}, {177, 446}, {194, 480}, {177, 453}, {117, 484}, {179, 374}, {170, 437}, {162, 516}, {190, 541}, {205, 501}, {187, 558}, {193, 531}, {224, 411}, {249, 371}, {235, 405}, {172, 550}, {197, 591}, {188, 652}, {178, 627}, {179, 623}, {185, 660}, {228, 577}, {224, 556}, {186, 576}, {186, 670}, {182, 678}, {164, 796}, {167, 785}, {201, 772}, {216, 719}, {230, 748}, {195, 588}, {180, 701}, {181, 717}, {164, 831}, {167, 915}, {219, 931}, {239, 891}, {250, 837}, {171, 626}, {184, 675}, {166, 760}, {213, 793}, {231, 906}, {369, 913}, {366, 888}, {288, 899}, {155, 695}, {91, 796}, {183, 786}, {134, 905}, {190, 962}, {343, 901}, {167, 990}, {284, 852}, {144, 784}, {197, 784}, {220, 836}, {292, 808}, {304, 844}, {504, 769}, {478, 775}, {419, 773} },
            { {-55, -129}, {49, -74}, {-38, -43}, {-275, -46}, {-86, -156}, {-248, -15}, {25, -90}, {46, -228}, {11, -25}, {-48, -2}, {-104, 35}, {-243, 61}, {-158, 40}, {-179, 47}, {-11, -24}, {32, -88}, {-111, -21}, {-98, 37}, {-91, 62}, {-140, 103}, {-98, 89}, {-119, 66}, {-29, 5}, {-98, -25}, {-35, -44}, {8, 52}, {69, 89}, {-97, 139}, {-17, 120}, {-33, 96}, {0, 52}, {-186, -1}, {-96, 41}, {107, 84}, {78, 118}, {16, 122}, {-21, 129}, {96, 111}, {7, 107}, {-193, 28}, {-73, 41}, {127, 115}, {219, 88}, {89, 83}, {200, 54}, {264, 103}, {150, 123}, {-136, 23}, {15, -98}, {26, 89}, {246, 30}, {94, 23}, {49, 41}, {138, 56}, {15, 119}, {-50, -83}, {61, -444}, {121, -168}, {121, -124}, {19, -58}, {58, -106}, {45, -77}, {91, -70}, {-6, -395} }
        };

        static constexpr Score past_pawn[8] = {
            {0, 0}, {-28, 2}, {-40, 16}, {-30, 78}, {35, 136}, {69, 293}, {251, 389}, {0, 0}
        };

        static constexpr Score piece_type_value[NO_PIECE_TYPES] = {
            {95, 302}, {482, 1119}, {539, 1192}, {746, 1851}, {1786, 3199}, {0, 0}
        };
    };

    namespace Bitboard {
        static constexpr chess::Bitboard NOT_H_FILE = chess::Bitboard(0x7f7f7f7f7f7f7f7fULL);
        static constexpr chess::Bitboard NOT_A_FILE = chess::Bitboard(0xfefefefefefefefeULL);
        
        inline chess::Bitboard north_fill(chess::Bitboard bb) {
            bb |= (bb << 8);
            bb |= (bb << 16);
            bb |= (bb << 32);

            return bb;
        }

        inline chess::Bitboard south_fill(chess::Bitboard bb) {
            bb |= (bb >> 8);
            bb |= (bb >> 16);
            bb |= (bb >> 32);

            return bb;
        }
    }

    [[nodiscard]] inline int piece_value(const chess::PieceType type) {
        return Evaluation::Params::piece_type_value[static_cast<int>(type.internal())].mg;
    }
    
    [[nodiscard]] inline int calculate_game_phase(const chess::Board& board) {
        int game_phase = 0;

        for (size_t type_idx = 0; type_idx < Params::NO_PIECE_TYPES; type_idx++) {
            const chess::PieceType type = Params::piece_types[type_idx];
            const int phase_value       = Params::piece_type_phase[type_idx];

            chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
            chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);
            
            const int white_pieces = white_bb.count();
            const int black_pieces = black_bb.count();
            
            game_phase += phase_value * (white_pieces + black_pieces);
        }

        return game_phase;
    }

    [[nodiscard]] inline int calculate_tapered_eval(const Score total, const int game_phase) {
        return (total.mg * game_phase + total.eg * (Params::MAX_PHASE - game_phase)) / Params::MAX_PHASE;
    };

    [[nodiscard]] inline Score evaluate_psts(const chess::Board& board, EvalTrace* trace = nullptr) {
        Score total = {0, 0};

        for (size_t type_idx = 0; type_idx < Params::NO_PIECE_TYPES; type_idx++) {
            const chess::PieceType type    = Params::piece_types[type_idx];
            
            chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
            chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);
            
            while (white_bb) {
                const int pst_sq = white_bb.pop();
                total += Params::piece_type_pst[type_idx][pst_sq];

                if (trace) {
                    trace->pst[EvalTrace::WHITE][type_idx][pst_sq] += 1;
                }
            }

            while (black_bb) {
                const int pst_sq = black_bb.pop() ^ 56;
                total -= Params::piece_type_pst[type_idx][pst_sq];

                if (trace) {
                    trace->pst[EvalTrace::BLACK][type_idx][pst_sq] += 1;
                }
            }
        }

        return total;
    }

    [[nodiscard]] inline Score evaluate_material(const chess::Board& board, EvalTrace* trace = nullptr) {
        Score total = {0, 0};

        for (size_t type_idx = 0; type_idx < Params::NO_PIECE_TYPES; type_idx++) {
            const chess::PieceType type    = Params::piece_types[type_idx];
            
            chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
            chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);

            const Score   piece_value = Params::piece_type_value[type_idx];

            const int white_pieces = white_bb.count();
            const int black_pieces = black_bb.count();

            total += piece_value * white_pieces;
            total -= piece_value * black_pieces;

            if (trace) {
                trace->material[EvalTrace::WHITE][type_idx] += white_pieces;
                trace->material[EvalTrace::BLACK][type_idx] += black_pieces;
            }
        }

        return total;
    }

    [[nodiscard]] inline Score pawn_evaluation(const chess::Board& board, EvalTrace* trace = nullptr){
        // Past Pawn Evaluation is done using my PPE algorithm (https://www.reddit.com/r/chessprogramming/comments/1k416zx/ppe_parallel_pawn_evaluation_algorithm_for_hce/).
        const chess::Bitboard white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
        const chess::Bitboard black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

        const chess::Bitboard white_pawns_north_fill = Bitboard::north_fill(white_pawns);
        const chess::Bitboard black_pawns_south_fill = Bitboard::south_fill(black_pawns);

        const chess::Bitboard white_adjacent = ((white_pawns_north_fill & Bitboard::NOT_H_FILE) << 9) | ((white_pawns_north_fill & Bitboard::NOT_A_FILE) << 7);
        const chess::Bitboard black_adjacent = ((black_pawns_south_fill & Bitboard::NOT_A_FILE) >> 9) | ((black_pawns_south_fill & Bitboard::NOT_H_FILE) >> 7);

        Score total = {0, 0};

        chess::Bitboard white_passed = (~(black_pawns_south_fill | black_adjacent) & white_pawns);
        chess::Bitboard black_passed = (~(white_pawns_north_fill | white_adjacent) & black_pawns);

        while (white_passed) { 
            const int rank = white_passed.pop() / 8;
            total += Params::past_pawn[rank]; 

            if (trace) {
                trace->passed_pawn[EvalTrace::WHITE][rank] += 1;
            }
        }

        while (black_passed) { 
            const int rank = 7 - (black_passed.pop() / 8);
            total -= Params::past_pawn[rank];
            
            if (trace) {
                trace->passed_pawn[EvalTrace::BLACK][rank] += 1;
            }
        }

        return total;
    }

    int evaluation(const chess::Board& board);
}
#endif