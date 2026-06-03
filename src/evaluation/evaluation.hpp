#pragma once

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
        int piece_mobility[2][6] = {};
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

        static constexpr Score piece_mobility[NO_PIECE_TYPES] {
            {0, 0}, {9, 10}, {10, 20}, {10, 10}, {5, 12}, {0, 0}
        };

        static constexpr Score past_pawn[8] = {
            {0, 0}, {-33, 2}, {-44, 17}, {-33, 80}, {31, 137}, {62, 293}, {170, 343}, {0, 0}
        };

        static constexpr Score piece_type_value[6] = {
            {113, 214}, {491, 1015}, {556, 978}, {642, 1403}, {1759, 2734}, {0, 0}
        };

        static constexpr Score piece_type_pst[NO_PIECE_TYPES][64] = {
            { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {8, 162}, {18, 161}, {0, 156}, {15, 149}, {8, 177}, {82, 170}, {129, 153}, {50, 88}, {8, 149}, {28, 159}, {17, 128}, {23, 128}, {52, 153}, {38, 144}, {102, 150}, {51, 111}, {4, 173}, {33, 178}, {28, 122}, {37, 118}, {56, 120}, {44, 133}, {72, 155}, {28, 129}, {2, 214}, {44, 200}, {28, 158}, {63, 98}, {85, 114}, {93, 125}, {71, 175}, {17, 160}, {1, 296}, {71, 262}, {95, 190}, {87, 102}, {115, 93}, {228, 139}, {110, 195}, {48, 201}, {164, 323}, {171, 312}, {162, 252}, {206, 154}, {190, 143}, {108, 187}, {-184, 310}, {-182, 318}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
            { {-212, -55}, {30, -52}, {-46, 64}, {20, 89}, {35, 77}, {35, 51}, {37, -12}, {-176, -13}, {-39, 51}, {-3, 102}, {22, 83}, {62, 113}, {48, 126}, {50, 71}, {10, 97}, {31, 53}, {7, 10}, {40, 112}, {52, 125}, {70, 189}, {90, 177}, {68, 123}, {59, 118}, {35, 17}, {40, 99}, {47, 182}, {79, 220}, {80, 246}, {97, 244}, {83, 242}, {127, 194}, {72, 136}, {65, 109}, {81, 187}, {116, 223}, {143, 263}, {106, 270}, {156, 249}, {105, 232}, {134, 145}, {-17, 97}, {76, 162}, {134, 203}, {163, 210}, {242, 184}, {272, 213}, {164, 175}, {159, 85}, {31, 45}, {17, 122}, {110, 133}, {144, 214}, {163, 197}, {247, 89}, {9, 128}, {97, 49}, {-327, -100}, {-161, 99}, {-221, 205}, {36, 137}, {170, 127}, {-155, 213}, {-15, 102}, {-227, -139} },
            { {77, 122}, {86, 155}, {47, 121}, {9, 152}, {-3, 171}, {37, 138}, {48, 149}, {57, 141}, {69, 127}, {75, 136}, {83, 148}, {48, 171}, {55, 160}, {69, 129}, {104, 146}, {78, 58}, {45, 157}, {65, 180}, {61, 194}, {72, 197}, {64, 205}, {69, 174}, {58, 163}, {79, 167}, {30, 157}, {61, 185}, {64, 205}, {95, 201}, {116, 183}, {43, 205}, {58, 186}, {64, 163}, {5, 191}, {81, 204}, {79, 188}, {162, 183}, {106, 213}, {126, 188}, {76, 232}, {54, 202}, {29, 186}, {53, 219}, {125, 176}, {97, 193}, {138, 185}, {163, 216}, {149, 210}, {90, 209}, {-50, 204}, {30, 207}, {23, 203}, {0, 222}, {29, 216}, {86, 190}, {15, 196}, {49, 172}, {-17, 216}, {-91, 247}, {-227, 254}, {-151, 263}, {-132, 233}, {-143, 239}, {117, 181}, {-12, 219} },
            { {88, 564}, {110, 579}, {129, 590}, {146, 576}, {134, 581}, {130, 594}, {175, 558}, {112, 515}, {-4, 582}, {73, 563}, {86, 584}, {87, 582}, {87, 579}, {123, 558}, {148, 551}, {4, 599}, {43, 580}, {64, 614}, {55, 617}, {75, 610}, {70, 617}, {77, 612}, {165, 587}, {94, 568}, {52, 634}, {53, 670}, {63, 680}, {85, 665}, {79, 662}, {84, 670}, {139, 650}, {121, 622}, {85, 670}, {126, 676}, {147, 685}, {200, 670}, {170, 679}, {183, 668}, {231, 638}, {183, 646}, {97, 692}, {194, 668}, {195, 692}, {252, 663}, {295, 644}, {361, 655}, {435, 587}, {245, 647}, {155, 694}, {139, 714}, {206, 705}, {264, 705}, {245, 714}, {360, 637}, {248, 664}, {313, 638}, {247, 666}, {240, 682}, {187, 703}, {217, 689}, {239, 694}, {348, 683}, {365, 673}, {379, 664} },
            { {363, 619}, {372, 592}, {392, 554}, {400, 628}, {389, 577}, {322, 623}, {380, 522}, {355, 567}, {345, 645}, {357, 661}, {376, 629}, {369, 674}, {375, 654}, {404, 549}, {417, 520}, {410, 558}, {338, 682}, {357, 723}, {358, 757}, {346, 742}, {349, 747}, {355, 794}, {384, 742}, {385, 722}, {342, 706}, {338, 781}, {340, 783}, {323, 888}, {324, 890}, {356, 906}, {369, 872}, {384, 917}, {335, 730}, {327, 816}, {315, 822}, {306, 922}, {313, 1010}, {357, 1054}, {393, 1041}, {400, 1004}, {308, 775}, {313, 809}, {296, 872}, {333, 910}, {359, 1014}, {499, 1050}, {511, 1040}, {436, 1071}, {292, 843}, {214, 924}, {300, 900}, {251, 1022}, {307, 1081}, {459, 1033}, {301, 1118}, {433, 1007}, {294, 892}, {307, 914}, {316, 965}, {385, 932}, {402, 974}, {596, 918}, {604, 912}, {564, 895} },
            { {-61, -124}, {58, -76}, {-17, -43}, {-239, -46}, {-70, -137}, {-220, -16}, {31, -94}, {39, -224}, {0, -19}, {-59, 1}, {-115, 38}, {-248, 62}, {-172, 43}, {-184, 48}, {-27, -20}, {15, -83}, {-104, -18}, {-96, 37}, {-94, 62}, {-146, 102}, {-111, 89}, {-122, 65}, {-42, 8}, {-104, -20}, {-24, -44}, {19, 49}, {73, 86}, {-92, 135}, {-24, 117}, {-36, 93}, {-1, 52}, {-180, 0}, {-68, 36}, {130, 77}, {94, 112}, {32, 117}, {-8, 123}, {106, 107}, {24, 101}, {-173, 27}, {-34, 34}, {163, 106}, {256, 80}, {136, 73}, {225, 49}, {288, 97}, {175, 118}, {-111, 20}, {29, -100}, {88, 79}, {301, 21}, {173, 13}, {108, 34}, {175, 52}, {68, 113}, {-25, -81}, {89, -432}, {133, -159}, {148, -118}, {91, -60}, {86, -97}, {94, -73}, {109, -63}, {-16, -380} }
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
        
    inline constexpr int piece_value(const chess::PieceType type) {
        return Params::piece_type_value[static_cast<int>(type.internal())].mg;
    }

    template <chess::Color::underlying c>
    [[nodiscard]] inline chess::Bitboard get_all_pawn_attacks(const chess::Bitboard pawns) {
        return chess::attacks::pawnLeftAttacks<c>(pawns) | chess::attacks::pawnRightAttacks<c>(pawns);
    }

    [[nodiscard]] inline chess::Bitboard get_piece_type_attack(const chess::PieceType type, const int sq, const chess::Bitboard occ) {
        switch (type.internal()) {
        case chess::PieceType::KNIGHT:
            return chess::attacks::knight(sq);
        case chess::PieceType::BISHOP:
            return chess::attacks::bishop(sq, occ);
        case chess::PieceType::ROOK:
            return chess::attacks::rook(sq, occ);
        case chess::PieceType::QUEEN:
            return chess::attacks::queen(sq, occ);
        case chess::PieceType::KING:
            return chess::attacks::king(sq);
        default:
            return chess::Bitboard(0ULL);
        }
    }

    [[nodiscard]] inline uint8_t calculate_game_phase(const chess::Board& board) {
        uint8_t game_phase = 0;

        for (size_t type_idx = 0; type_idx < Params::NO_PIECE_TYPES; type_idx++) {
            const chess::PieceType type = Params::piece_types[type_idx];
            const uint8_t phase_value   = Params::piece_type_phase[type_idx];

            chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
            chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);
            
            const int white_pieces = white_bb.count();
            const int black_pieces = black_bb.count();
            
            game_phase += phase_value * (white_pieces + black_pieces);
        }

        return game_phase;
    }

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

            const Score piece_value = Params::piece_type_value[type_idx];

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
            const int past_square = white_passed.pop();
            const int rank = past_square / 8;

            // Past pawn ranks
            total += Params::past_pawn[rank]; 

            if (trace) {
                trace->passed_pawn[EvalTrace::WHITE][rank] += 1;
            }
        }

        while (black_passed) {
            const int past_square = black_passed.pop();
            const int rank = 7 - (past_square / 8);

            total -= Params::past_pawn[rank];

            if (trace) {
                trace->passed_pawn[EvalTrace::BLACK][rank] += 1;
            }
        }

        return total;
    }

    [[nodiscard]] inline Score mobility_evaluation(const chess::Board& board, EvalTrace* trace = nullptr) {
        Score total = {0, 0};

        const chess::Bitboard white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
        const chess::Bitboard black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

        const chess::Bitboard white_pawn_attacks = get_all_pawn_attacks<chess::Color::WHITE>(white_pawns);
        const chess::Bitboard black_pawn_attacks = get_all_pawn_attacks<chess::Color::BLACK>(black_pawns);

        const chess::Bitboard white_occ = board.us(chess::Color::WHITE);
        const chess::Bitboard black_occ = board.us(chess::Color::BLACK);
        
        const chess::Bitboard occ = white_occ | black_occ;

        for (size_t type_idx = 1; type_idx < 5; type_idx++) { // Evaluates mobility for Knights, Bishops, Rooks and Queens
            const chess::PieceType type    = Params::piece_types[type_idx];
            
            chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
            chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);

            while (white_bb) {
                const int sq = white_bb.pop();
                const int mobility = (get_piece_type_attack(type, sq, occ) & ~(black_pawn_attacks | white_occ)).count();

                total += Params::piece_mobility[type_idx] * mobility;

                if (trace) {
                    trace->piece_mobility[EvalTrace::WHITE][type_idx] += mobility;
                }
            }

            while (black_bb) {
                const int sq = black_bb.pop();
                const int mobility = (get_piece_type_attack(type, sq, occ) & ~(white_pawn_attacks | black_occ)).count();

                total -= Params::piece_mobility[type_idx] * mobility;

                if (trace) {
                    trace->piece_mobility[EvalTrace::BLACK][type_idx] += mobility;
                }
            }
        }
        

        return total;
    }

    int evaluation(const chess::Board& board);
}
#endif