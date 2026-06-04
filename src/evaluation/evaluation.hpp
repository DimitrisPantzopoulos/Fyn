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

        int rook_on_open_file[2] = {};
        int rook_on_semi_open_file[2] = {};
        int connected_rooks[2]   = {};
        int rook_on_seventh[2]   = {};
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

        static constexpr Score rook_on_open_file = {80, 4};
        static constexpr Score rook_on_semi_open_file = {23, 24};
        static constexpr Score connected_rooks = {-1, 89};
        static constexpr Score rook_on_seventh = {139, 424};

        static constexpr Score piece_mobility[NO_PIECE_TYPES] {
            {0, 0}, {9, 10}, {10, 20}, {6, 10}, {4, 12}, {0, 0}
        };

        static constexpr Score past_pawn[8] = {
            {0, 0}, {-34, 3}, {-44, 18}, {-33, 80}, {33, 138}, {66, 294}, {185, 344}, {0, 0}
        };

        static constexpr Score piece_type_value[6] = {
            {122, 214}, {507, 1005}, {573, 974}, {643, 1369}, {1802, 2715}, {0, 0}
        };

        static constexpr Score piece_type_pst[NO_PIECE_TYPES][64] = {
            { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {7, 165}, {16, 164}, {-3, 158}, {12, 150}, {6, 177}, {86, 169}, {130, 153}, {48, 89}, {11, 151}, {27, 161}, {14, 130}, {21, 130}, {52, 153}, {42, 143}, {103, 149}, {51, 112}, {10, 175}, {34, 179}, {28, 124}, {39, 119}, {59, 120}, {52, 130}, {73, 155}, {29, 129}, {12, 216}, {47, 202}, {31, 159}, {68, 99}, {90, 114}, {105, 122}, {73, 175}, {21, 160}, {14, 298}, {75, 263}, {104, 191}, {98, 102}, {126, 93}, {241, 137}, {112, 195}, {55, 201}, {180, 323}, {172, 313}, {174, 249}, {221, 150}, {198, 140}, {117, 185}, {-198, 312}, {-181, 320}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
            { {-211, -46}, {26, -28}, {-45, 74}, {19, 102}, {35, 88}, {40, 61}, {35, 4}, {-171, -3}, {-38, 61}, {1, 109}, {26, 91}, {66, 121}, {52, 135}, {53, 81}, {17, 106}, {37, 63}, {11, 16}, {45, 119}, {57, 132}, {76, 196}, {96, 184}, {74, 130}, {67, 125}, {41, 23}, {44, 105}, {54, 187}, {86, 226}, {88, 252}, {104, 250}, {90, 249}, {135, 199}, {79, 143}, {67, 114}, {88, 192}, {121, 228}, {149, 268}, {113, 275}, {163, 254}, {113, 238}, {142, 150}, {-20, 102}, {75, 168}, {137, 209}, {168, 214}, {246, 189}, {279, 218}, {167, 181}, {165, 91}, {33, 49}, {18, 127}, {113, 139}, {148, 219}, {166, 202}, {249, 94}, {9, 134}, {101, 53}, {-316, -97}, {-158, 106}, {-222, 212}, {34, 145}, {173, 133}, {-148, 219}, {-11, 108}, {-221, -135} },
            { {81, 124}, {87, 159}, {45, 136}, {8, 156}, {-4, 175}, {37, 145}, {49, 151}, {62, 142}, {71, 128}, {80, 136}, {88, 149}, {52, 171}, {58, 161}, {73, 130}, {111, 146}, {82, 58}, {47, 157}, {71, 180}, {66, 195}, {76, 198}, {69, 205}, {74, 175}, {64, 163}, {84, 167}, {35, 156}, {65, 184}, {68, 205}, {103, 200}, {122, 183}, {49, 205}, {65, 186}, {70, 162}, {7, 190}, {87, 202}, {82, 188}, {168, 182}, {112, 212}, {130, 188}, {83, 231}, {59, 201}, {29, 186}, {54, 218}, {127, 175}, {98, 192}, {141, 185}, {166, 215}, {152, 209}, {94, 208}, {-48, 203}, {34, 205}, {24, 203}, {0, 222}, {31, 215}, {86, 190}, {16, 196}, {56, 171}, {-2, 214}, {-88, 247}, {-223, 254}, {-149, 263}, {-128, 232}, {-133, 237}, {123, 181}, {0, 217} },
            { {104, 580}, {109, 592}, {121, 601}, {140, 580}, {136, 585}, {146, 605}, {179, 570}, {129, 534}, {12, 599}, {79, 583}, {81, 605}, {87, 600}, {92, 596}, {139, 574}, {159, 569}, {23, 615}, {60, 597}, {70, 632}, {56, 634}, {79, 623}, {79, 630}, {99, 626}, {184, 604}, {118, 585}, {66, 650}, {60, 686}, {64, 695}, {89, 678}, {87, 675}, {102, 683}, {160, 666}, {141, 639}, {93, 686}, {127, 693}, {142, 702}, {197, 684}, {170, 693}, {196, 681}, {243, 655}, {197, 663}, {94, 710}, {184, 688}, {182, 710}, {233, 682}, {280, 663}, {359, 671}, {436, 608}, {247, 667}, {-1, 292}, {-14, 312}, {50, 299}, {107, 298}, {90, 307}, {202, 235}, {93, 264}, {151, 241}, {231, 693}, {229, 705}, {177, 724}, {206, 709}, {229, 715}, {348, 704}, {366, 697}, {373, 690} },
            { {376, 627}, {380, 608}, {398, 575}, {406, 658}, {396, 596}, {333, 634}, {391, 532}, {367, 576}, {352, 654}, {372, 666}, {392, 625}, {383, 678}, {389, 659}, {416, 556}, {434, 529}, {420, 572}, {349, 687}, {374, 722}, {373, 760}, {361, 744}, {364, 750}, {370, 799}, {401, 746}, {400, 727}, {356, 707}, {354, 783}, {355, 786}, {338, 891}, {339, 894}, {372, 909}, {387, 875}, {401, 920}, {345, 734}, {341, 818}, {326, 827}, {320, 927}, {326, 1017}, {373, 1057}, {410, 1044}, {417, 1008}, {318, 778}, {322, 815}, {306, 879}, {341, 919}, {371, 1020}, {513, 1055}, {530, 1041}, {452, 1075}, {301, 849}, {226, 928}, {304, 912}, {255, 1035}, {312, 1093}, {465, 1043}, {312, 1126}, {445, 1015}, {312, 898}, {331, 915}, {340, 966}, {412, 932}, {428, 975}, {618, 922}, {623, 917}, {586, 899} },
            { {-53, -126}, {61, -79}, {-17, -46}, {-234, -46}, {-74, -126}, {-218, -16}, {30, -93}, {41, -225}, {7, -21}, {-51, 0}, {-107, 35}, {-245, 62}, {-168, 43}, {-180, 47}, {-21, -21}, {20, -85}, {-100, -20}, {-93, 36}, {-93, 61}, {-147, 102}, {-110, 89}, {-121, 65}, {-41, 8}, {-102, -20}, {-26, -46}, {17, 48}, {69, 85}, {-97, 134}, {-29, 117}, {-36, 93}, {-1, 51}, {-180, 0}, {-70, 34}, {127, 76}, {90, 111}, {26, 116}, {-13, 123}, {102, 106}, {23, 100}, {-174, 27}, {-38, 32}, {160, 104}, {255, 79}, {132, 73}, {224, 49}, {284, 97}, {172, 118}, {-113, 20}, {27, -103}, {85, 77}, {300, 20}, {174, 13}, {106, 34}, {171, 52}, {64, 113}, {-26, -82}, {88, -433}, {134, -160}, {148, -119}, {92, -60}, {85, -98}, {94, -73}, {111, -63}, {-16, -380} }
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

    [[nodiscard]] inline chess::Bitboard get_squares_as_files(const chess::Bitboard bb) {
        return Bitboard::north_fill(bb) | Bitboard::south_fill(bb);
    }

    [[nodiscard]] inline chess::Bitboard get_square_as_file(const int sq) {
        return get_squares_as_files(chess::Bitboard(1ULL << sq));
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

    [[nodiscard]] inline Score rook_evaluation(const chess::Board& board, EvalTrace* trace = nullptr) {
        const chess::Bitboard white_rooks = board.pieces(chess::PieceType::ROOK, chess::Color::WHITE);
        const chess::Bitboard black_rooks = board.pieces(chess::PieceType::ROOK, chess::Color::BLACK);

        const chess::Bitboard white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
        const chess::Bitboard black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

        const chess::Bitboard occ = board.occ();

        const chess::Bitboard white_pawn_files = get_squares_as_files(white_pawns);
        const chess::Bitboard black_pawn_files = get_squares_as_files(black_pawns);

        const chess::Bitboard all_pawn_files = white_pawn_files | black_pawn_files;

        const int white_open_file_rooks = (white_rooks & ~all_pawn_files).count();
        const int black_open_file_rooks = (black_rooks & ~all_pawn_files).count();

        const int white_semi_open_file_rooks =
            (white_rooks & ~white_pawn_files & black_pawn_files).count();

        const int black_semi_open_file_rooks =
            (black_rooks & ~black_pawn_files & white_pawn_files).count();

        const int white_7th_file_rooks = (white_rooks & chess::attacks::MASK_RANK[6]).count();
        const int black_7th_file_rooks = (black_rooks & chess::attacks::MASK_RANK[1]).count();

        const bool white_connected_rooks =
            white_rooks.count() >= 2 &&
            ((white_rooks & chess::attacks::rook(white_rooks.lsb(), occ)).count() >= 1);

        const bool black_connected_rooks =
            black_rooks.count() >= 2 &&
            ((black_rooks & chess::attacks::rook(black_rooks.lsb(), occ)).count() >= 1);

        Score total = {0, 0};

        total += Params::rook_on_open_file * (white_open_file_rooks - black_open_file_rooks);
        total += Params::rook_on_semi_open_file * (white_semi_open_file_rooks - black_semi_open_file_rooks);
        total += Params::rook_on_seventh   * (white_7th_file_rooks  - black_7th_file_rooks);
        total += Params::connected_rooks   * (white_connected_rooks - black_connected_rooks);

        if (trace){
            trace->rook_on_open_file[EvalTrace::WHITE] += white_open_file_rooks;
            trace->rook_on_open_file[EvalTrace::BLACK] += black_open_file_rooks;

            trace->rook_on_semi_open_file[EvalTrace::WHITE] += white_semi_open_file_rooks;
            trace->rook_on_semi_open_file[EvalTrace::BLACK] += black_semi_open_file_rooks;

            trace->rook_on_seventh[EvalTrace::WHITE] += white_7th_file_rooks;
            trace->rook_on_seventh[EvalTrace::BLACK] += black_7th_file_rooks;

            trace->connected_rooks[EvalTrace::WHITE] += white_connected_rooks;
            trace->connected_rooks[EvalTrace::BLACK] += black_connected_rooks;
        }

        return total;
    }

    int evaluation(const chess::Board& board);
}
#endif