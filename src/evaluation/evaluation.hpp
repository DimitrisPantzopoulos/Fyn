#pragma once

#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include "evaluation_helper.hpp"
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
        int connected_rooks[2]   = {};
        int rook_on_seventh[2]   = {};
        int rook_on_semi_open_file[2] = {};

        int supported_pawn[2]  = {};
        int backwards_pawn[2]  = {};
        int phalanx_pawn[2][8] = {};

        int isolated_pawn[2]   = {};
        int doubled_pawn[2]    = {};

        template<typename Feature>
        static void trace_scalar(Feature& feature, int white, int black) {
            feature[WHITE] += white;
            feature[BLACK] += black;
        }

        template<typename Feature>
        static void trace_array(Feature& feature, int side, int index, int value = 1) {
            feature[side][index] += value;
        }
    };

    namespace Params {
        static constexpr uint8_t NO_PIECE_TYPES = 6;
        static constexpr uint8_t MAX_PHASE      = 24;
        static constexpr chess::Color EVAL_COLORS[2] = {chess::Color::WHITE, chess::Color::BLACK};

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

        static constexpr Score piece_type_value[6] = {
            {126, 202}, {563, 1014}, {636, 989}, {711, 1377}, 
            {1945, 2741}, {0, 0}
        };

        static constexpr Score piece_type_pst[6][64] = {
            { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {18, 154}, {-2, 142}, {-7, 152}, {16, 138}, {6, 174}, {99, 154}, {114, 120}, {58, 75}, {8, 131}, {-21, 122}, {-7, 117}, {-1, 114}, {23, 131}, {30, 125}, {60, 96}, {34, 88}, {20, 154}, {-1, 146}, {16, 101}, {22, 95}, {49, 93}, {52, 105}, {50, 108}, {26, 108}, {34, 205}, {29, 179}, {31, 145}, {76, 80}, {101, 95}, {119, 101}, {67, 142}, {39, 147}, {38, 303}, {76, 263}, {126, 187}, {124, 92}, {158, 77}, {277, 128}, {130, 181}, {78, 204}, {200, 342}, {189, 323}, {192, 258}, {243, 154}, {214, 144}, {132, 192}, {-200, 321}, {-176, 341}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
            { {-203, -48}, {23, -24}, {-39, 81}, {25, 106}, {43, 94}, {49, 72}, {34, 9}, {-161, -6}, {-36, 65}, {4, 114}, {34, 99}, {72, 129}, {60, 145}, {56, 90}, {28, 110}, {44, 70}, {13, 19}, {55, 125}, {67, 141}, {88, 205}, {106, 194}, {85, 143}, {80, 133}, {46, 27}, {47, 105}, {57, 194}, {98, 236}, {101, 259}, {116, 256}, {102, 257}, {148, 204}, {85, 143}, {73, 117}, {97, 198}, {138, 238}, {169, 278}, {133, 286}, {181, 262}, {126, 243}, {157, 151}, {-26, 107}, {90, 173}, {156, 219}, {185, 221}, {268, 196}, {302, 224}, {182, 189}, {175, 93}, {22, 50}, {12, 132}, {116, 145}, {167, 226}, {179, 210}, {255, 97}, {2, 138}, {103, 56}, {-356, -88}, {-164, 115}, {-237, 221}, {33, 152}, {173, 140}, {-165, 231}, {-7, 116}, {-261, -136} },
            { {90, 130}, {97, 165}, {49, 146}, {23, 162}, {9, 180}, {47, 156}, {60, 157}, {77, 147}, {82, 132}, {91, 144}, {100, 158}, {62, 182}, {70, 172}, {82, 137}, {127, 151}, {95, 65}, {53, 162}, {85, 187}, {77, 205}, {88, 210}, {80, 216}, {88, 183}, {79, 171}, {92, 169}, {43, 163}, {71, 196}, {82, 216}, {116, 214}, {135, 194}, {62, 218}, {74, 194}, {83, 167}, {14, 201}, {99, 213}, {89, 203}, {183, 195}, {126, 224}, {135, 201}, {99, 240}, {69, 211}, {37, 194}, {63, 232}, {141, 188}, {112, 206}, {159, 199}, {182, 229}, {158, 224}, {107, 217}, {-61, 216}, {36, 214}, {30, 216}, {2, 235}, {37, 228}, {88, 205}, {14, 210}, {66, 177}, {-8, 224}, {-89, 260}, {-233, 267}, {-155, 277}, {-133, 246}, {-151, 252}, {127, 194}, {-4, 231} },
            { {122, 590}, {125, 602}, {138, 609}, {158, 587}, {155, 593}, {165, 614}, {199, 580}, {146, 543}, {27, 607}, {93, 593}, {97, 612}, {103, 605}, {110, 601}, {158, 580},{177, 574}, {39, 618}, {75, 609}, {87, 645}, {70, 647}, {95, 634}, {95, 641}, {121, 637}, {207, 615}, {135, 595}, {82, 662}, {73, 701}, {81, 706}, {103, 690}, {102, 687},{122, 695}, {183, 677}, {154, 652}, {111, 699}, {144, 706}, {162, 713}, {219, 694}, {187, 704}, {219, 693}, {267, 669}, {212, 680}, {114, 720}, {205, 698}, {201, 719}, {256, 690}, {303, 672}, {384, 680}, {460, 617}, {270, 680}, {4, 297}, {-12, 317}, {55, 303}, {114, 302}, {97, 311}, {206, 237}, {101, 267}, {158, 246}, {249, 705}, {249, 717}, {197, 733}, {229, 718}, {253, 725}, {365, 716}, {386, 708}, {392, 702} },
            { {413, 640}, {414, 624}, {432, 596}, {440, 678}, {433, 615}, {368, 654}, {429, 543}, {403, 586}, {386, 669}, {407, 681}, {429, 641}, {418, 701}, {423, 681}, {452, 578}, {469, 546}, {452, 594}, {382, 702}, {408, 742}, {407, 786}, {397, 768}, {399, 775}, {404, 827}, {439, 768}, {432, 745}, {387, 728}, {386, 807}, {390, 811}, {371, 918},{370, 925}, {412, 935}, {423, 899}, {435, 941}, {372, 756}, {373, 843}, {359, 853}, {354, 958}, {358, 1051}, {404, 1093}, {449, 1072}, {455, 1028}, {343, 801}, {354, 835}, {336, 908}, {376, 945}, {409, 1050}, {553, 1087}, {566, 1075}, {492, 1096}, {319, 876}, {242, 964}, {329, 942}, {282, 1067}, {341, 1125}, {497, 1071}, {332, 1162}, {478,1034}, {335, 932}, {367, 941}, {379, 992}, {451, 959}, {468, 1003}, {654, 948}, {650, 951}, {619, 929} },
            { {-54, -130}, {61, -84}, {-21, -47}, {-244, -44}, {-85, -123}, {-226, -13}, {28, -95}, {40, -228}, {11, -28}, {-53, -1}, {-112, 34}, {-249, 63}, {-174, 44}, {-179, 48}, {-24, -19}, {25, -84}, {-100, -24}, {-93, 33}, {-89, 58}, {-143, 100}, {-114, 89}, {-123, 67}, {-64, 15}, {-110, -18}, {-15, -52}, {31, 43}, {87, 80}, {-90, 132}, {-25, 115}, {-26, 92}, {-8, 54}, {-184, -1}, {-60, 28}, {144, 72}, {105, 108}, {35, 115}, {1, 121}, {111, 106}, {30, 100}, {-175, 27}, {-24, 28}, {178, 102}, {271, 77}, {137, 73}, {230, 50}, {285, 99}, {175, 121}, {-106, 21}, {23, -101}, {89, 76}, {298, 22}, {180, 15}, {104, 38}, {178, 53}, {66, 116}, {-25, -83}, {85, -430}, {132, -156}, {143, -122}, {92, -60}, {86, -95}, {94, -69}, {109, -57}, {-24, -370} }
        };

        static constexpr Score past_pawn[8] = {
            {0, 0}, {-20, 40}, {-24, 60}, {-15, 119}, 
            {48, 164}, {77, 312}, {213, 359}, {0, 0}
        };

        static constexpr Score piece_mobility[6] = {
            {0, 0}, {8, 8}, {10, 19}, {7, 10}, {4, 12}, {0, 0}
        };
        
        static constexpr Score rook_on_open_file = {87, 8};
        static constexpr Score rook_on_semi_open_file = {38, 26};
        static constexpr Score connected_rooks = {-2, 91};
        static constexpr Score rook_on_seventh = {150, 429};
        static constexpr Score isolated_pawn = {-12, -24};
        static constexpr Score doubled_pawn = {-24, -54};
        static constexpr Score supported_pawn = {40, 26};
        static constexpr Score backwards_pawn = {-1, -12};
        static constexpr Score phalanx_pawn[8] = {
            {0, 0}, {7, -2}, {19, 14}, {25, 39}, 
            {40, 111}, {89, 204}, {47, 461}, {0, 0}
        };
    };
        
    inline constexpr int piece_value(const chess::PieceType type) {
        return Params::piece_type_value[static_cast<int>(type.internal())].mg;
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

        const EvaluationHelper::PawnFeatures white = EvaluationHelper::get_pawn_features<chess::Color::WHITE>(white_pawns, black_pawns);
        const EvaluationHelper::PawnFeatures black = EvaluationHelper::get_pawn_features<chess::Color::BLACK>(black_pawns, white_pawns);

        Score total = {0, 0};

        chess::Bitboard white_passed = white.passed;
        while (white_passed) {
            const int sq = white_passed.pop();
            const int rank = sq / 8;

            total += Params::past_pawn[rank];

            if (trace) {
                EvalTrace::trace_array(trace->passed_pawn, EvalTrace::WHITE, rank);
            }
        }

        chess::Bitboard black_passed = black.passed;
        while (black_passed) {
            const int sq = black_passed.pop();
            const int rank = 7 - (sq / 8);

            total -= Params::past_pawn[rank];

            if (trace) {
                EvalTrace::trace_array(trace->passed_pawn, EvalTrace::BLACK, rank);
            }
        }

        chess::Bitboard white_phalanx = white.phalanx;
        while (white_phalanx) {
            const int sq = white_phalanx.pop();
            const int rank = sq / 8;

            total += Params::phalanx_pawn[rank];

            if (trace) {
                EvalTrace::trace_array(trace->phalanx_pawn, EvalTrace::WHITE, rank);
            }
        }

        chess::Bitboard black_phalanx = black.phalanx;
        while (black_phalanx) {
            const int sq = black_phalanx.pop();
            const int rank = 7 - (sq / 8);

            total -= Params::phalanx_pawn[rank];

            if (trace) {
                EvalTrace::trace_array(trace->phalanx_pawn, EvalTrace::BLACK, rank);
            }
        }
        
        total += Params::isolated_pawn  * (white.isolated  - black.isolated);
        total += Params::doubled_pawn   * (white.doubled   - black.doubled);
        total += Params::backwards_pawn * (white.backwards - black.backwards);
        total += Params::supported_pawn * (white.supported - black.supported);

        if (trace) {
            EvalTrace::trace_scalar(trace->isolated_pawn,  white.isolated,  black.isolated);
            EvalTrace::trace_scalar(trace->doubled_pawn,   white.doubled,   black.doubled);
            EvalTrace::trace_scalar(trace->backwards_pawn, white.backwards, black.backwards);
            EvalTrace::trace_scalar(trace->supported_pawn, white.supported, black.supported);
        }

        return total;
    }

    [[nodiscard]] inline Score mobility_evaluation(const chess::Board& board, EvalTrace* trace = nullptr) {
        Score total = {0, 0};

        const chess::Bitboard white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
        const chess::Bitboard black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

        const chess::Bitboard white_pawn_attacks = EvaluationHelper::get_all_pawn_attacks<chess::Color::WHITE>(white_pawns);
        const chess::Bitboard black_pawn_attacks = EvaluationHelper::get_all_pawn_attacks<chess::Color::BLACK>(black_pawns);

        const chess::Bitboard white_occ = board.us(chess::Color::WHITE);
        const chess::Bitboard black_occ = board.us(chess::Color::BLACK);
        
        const chess::Bitboard occ = white_occ | black_occ;

        for (size_t type_idx = 1; type_idx < 5; type_idx++) { // Evaluates mobility for Knights, Bishops, Rooks and Queens
            const chess::PieceType type    = Params::piece_types[type_idx];
            
            chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
            chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);

            while (white_bb) {
                const int sq = white_bb.pop();
                const int mobility = (EvaluationHelper::get_piece_type_attack(type, sq, occ) & ~(black_pawn_attacks | white_occ)).count();

                total += Params::piece_mobility[type_idx] * mobility;

                if (trace) {
                    trace->piece_mobility[EvalTrace::WHITE][type_idx] += mobility;
                }
            }

            while (black_bb) {
                const int sq = black_bb.pop();
                const int mobility = (EvaluationHelper::get_piece_type_attack(type, sq, occ) & ~(white_pawn_attacks | black_occ)).count();

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

        const chess::Bitboard white_pawn_files = EvaluationHelper::get_squares_as_files(white_pawns);
        const chess::Bitboard black_pawn_files = EvaluationHelper::get_squares_as_files(black_pawns);

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
            EvalTrace::trace_scalar(trace->rook_on_open_file, white_open_file_rooks, black_open_file_rooks);
            EvalTrace::trace_scalar(trace->rook_on_semi_open_file, white_semi_open_file_rooks, black_semi_open_file_rooks);
            EvalTrace::trace_scalar(trace->rook_on_seventh, white_7th_file_rooks, black_7th_file_rooks);
            EvalTrace::trace_scalar(trace->connected_rooks, white_connected_rooks, black_connected_rooks);
        }

        return total;
    }

    int evaluation(const chess::Board& board);
}
#endif