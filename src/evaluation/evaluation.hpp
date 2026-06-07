#pragma once

#ifndef EVALUATION_HPP
#define EVALUATION_HPP

#include "evaluation_helper.hpp"
#include "include/chess.hpp"
#include <algorithm>
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

        int king_danger[2]          = {};
        int king_shield[2]          = {};
        int king_open_files[2]      = {};
        int king_semi_open_files[2] = {};

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
        static constexpr Score piece_type_value[6] = {
            {123, 213}, {599, 994}, {678, 976}, {764, 1358}, 
            {2013, 2723}, {0, 0}
        };

        static constexpr Score piece_type_pst[6][64] = {
            { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {22, 146}, {10, 132}, {0, 144}, {13, 138}, {6, 174}, {63, 176}, {86, 137}, {18, 91}, {11, 124}, {-9,113}, {0, 112}, {2, 113}, {25, 134}, {10, 141}, {38, 110}, {8, 98}, {25, 146}, {11, 137}, {26, 95}, {27, 94}, {55, 95}, {34, 121}, {30, 122}, {2, 118}, {42, 196}, {42, 170}, {42, 139}, {81, 78}, {107, 97}, {107, 115}, {52, 154}, {14, 157}, {47, 296}, {89, 257}, {137, 184}, {131, 93}, {166, 82}, {274, 140}, {127, 189}, {55, 212}, {217, 333}, {200, 318}, {208, 254}, {251, 155}, {224, 147}, {126, 200}, {-230, 331}, {-194, 344}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
            { {-197, -35}, {30, -20}, {-29, 85}, {35, 110}, {56, 99}, {63, 79}, {40, 16}, {-147, 5}, {-21, 73}, {17, 118}, {44, 104}, {82, 134}, {68, 150}, {70, 96}, {47, 113}, {59, 77}, {23, 24}, {66, 129}, {77, 143}, {100, 206}, {118, 194}, {92, 146}, {88, 136}, {56, 34}, {59, 107}, {72, 194}, {111, 234}, {113, 255}, {130, 250}, {110, 249}, {159, 196}, {94, 135}, {84, 116}, {109, 197}, {152, 233}, {181, 269}, {124, 277}, {182, 250}, {119, 234}, {145, 141}, {-15, 104}, {103, 170}, {171, 212}, {179, 213}, {262, 183}, {286, 216}, {180, 177}, {142, 87}, {35, 47}, {25, 129}, {130, 136}, {159, 221}, {163, 202}, {238, 85}, {10, 126}, {97, 44}, {-376, -81}, {-149, 110}, {-229, 215}, {8, 148}, {133, 137}, {-198, 225}, {-14, 106}, {-281, -143} },
            { {99, 122}, {99, 161}, {54, 142}, {31, 162}, {20, 180}, {62, 157}, {63, 161}, {86, 143}, {88, 125}, {101, 139}, {106, 154}, {71, 181}, {77, 174}, {92, 139}, {143, 148}, {104, 62}, {60, 155}, {92, 181}, {86, 199}, {96, 205}, {90, 214}, {95, 184}, {90, 171}, {105, 162}, {54, 160}, {79, 189}, {88, 210}, {123, 206}, {138, 188}, {73, 213},{83, 190}, {92, 160}, {24, 197}, {112, 210}, {91, 198}, {178, 189}, {129, 215}, {131, 196}, {109, 234}, {53, 206}, {48, 192}, {76, 228}, {155, 184}, {109, 200}, {159, 189}, {179, 223}, {160, 214}, {90, 213}, {-52, 212}, {49, 211}, {44, 210}, {19, 225}, {35, 220}, {90, 194}, {11, 201}, {-10, 185}, {-7, 222}, {-80, 254}, {-231, 258}, {-149, 265}, {-157, 239}, {-174, 246}, {87, 192}, {-31, 225} },
            { {146, 597}, {150, 608}, {163, 614}, {183, 591}, {179, 598}, {189, 614}, {214, 580}, {177, 546}, {53, 612}, {121, 596}, {126, 614}, {132, 606}, {137, 602}, {185, 575}, {203, 569}, {60, 618}, {100, 613}, {115, 647}, {99, 647}, {125, 633}, {122, 639}, {138, 633}, {214, 616}, {149, 598}, {109, 663}, {101, 700}, {111, 705}, {134, 687}, {132, 683}, {143, 688}, {181, 680}, {166, 651}, {140, 696}, {172, 703}, {194, 708}, {253, 687}, {216, 698}, {246, 684}, {268, 668}, {223, 675}, {141, 716}, {234, 693}, {227,714}, {285, 682}, {330, 664}, {379, 681}, {457, 615}, {267, 678}, {17, 293}, {-6, 315}, {59, 301}, {109, 304}, {92, 312}, {201, 233}, {98, 261}, {159, 241}, {271, 702}, {265, 715}, {204, 734}, {234, 719}, {246, 728}, {365, 716}, {391, 708}, {398, 702} },
            { {436, 646}, {427, 636}, {451, 623}, {460, 713}, {454, 657}, {394, 673}, {440, 559}, {425, 580}, {404, 685}, {430, 690}, {448, 654}, {440, 727}, {445, 719}, {474, 594}, {492, 558}, {465, 594}, {397, 705}, {428, 757}, {429, 790}, {410, 774}, {420, 792}, {420, 826}, {444, 762}, {435, 738}, {406, 757}, {402, 808}, {402, 814}, {393, 901},{378, 918}, {421, 917}, {426, 882}, {434, 919}, {391, 786}, {394, 872}, {365, 851}, {352, 949}, {362, 1034}, {397, 1080}, {448, 1065}, {433, 1011}, {366, 815}, {383, 842}, {365, 912}, {382, 930}, {412, 1034}, {550, 1075}, {561, 1059}, {463, 1091}, {341, 860}, {258, 948}, {344, 922}, {291, 1048}, {338, 1117}, {508, 1051}, {356, 1126}, {481,1011}, {338, 919}, {375, 926}, {378, 979}, {437, 956}, {457, 995}, {648, 937}, {622, 952}, {586, 932} },
            { {-98, -145}, {8, -81}, {-51, -30}, {-240, -28}, {-92, -114}, {-238, -14}, {-4, -106}, {-1, -257}, {-37, -41}, {-79, 4}, {-113, 50}, {-234, 78}, {-164, 55}, {-181, 47}, {-36, -32}, {-15, -100}, {-144, -31}, {-109, 43}, {-69, 75}, {-102, 119}, {-71, 99}, {-111, 64}, {-67, 2}, {-136, -41}, {-75, -51}, {38, 57}, {134, 99}, {-36, 158}, {16, 130}, {-5, 91}, {-1, 42}, {-215, -20}, {-105, 25}, {177, 78}, {163, 129}, {90, 139}, {55, 134}, {149, 109}, {58, 88}, {-199, 7}, {-46, 15}, {225, 108}, {329, 93}, {191,93}, {272, 64}, {324, 98}, {227, 106}, {-123, -4}, {12, -113}, {119, 86}, {338, 41}, {205, 40}, {142, 52}, {227, 50}, {124, 100}, {-23, -112}, {70, -425}, {143, -131}, {157, -91}, {113, -31}, {98, -72}, {109, -58}, {117, -57}, {-37, -373} }
        };

        static constexpr Score past_pawn[8] = {
            {0, 0}, {-14, 39}, {-22, 61}, {-15, 122}, 
            {49, 168}, {81, 313}, {225, 359}, {0, 0}
        };

        static constexpr Score piece_mobility[6] = {
            {0, 0}, {9, 9}, {9, 18}, {6, 10}, {3, 10}, {0, 0}
        };
        static constexpr Score rook_on_open_file = {87, 11};
        static constexpr Score rook_on_semi_open_file = {35, 29};
        static constexpr Score connected_rooks = {0, 85};
        static constexpr Score rook_on_seventh = {154, 430};
        static constexpr Score isolated_pawn = {-11, -25};
        static constexpr Score doubled_pawn = {-7, -73};
        static constexpr Score supported_pawn = {41, 26};
        static constexpr Score backwards_pawn = {-2, -11};
        static constexpr Score phalanx_pawn[8] = {
            {0, 0}, {6, 0}, {19, 14}, {25, 41}, 
            {42, 114}, {98, 203}, {62, 441}, {0, 0}
        };

        static constexpr Score king_danger[16] = {
            {83, 27}, {0, 0}, {74, 29}, {64, 39}, 
            {21, 26}, {56, -8}, {-52, 33}, {0, -129}, 
            {-66, -60}, {-126, -343}, {-274, -76}, {-326, -58}, 
            {-478, -171}, {-473, -144}, {-79, -24}, {-69, -12}
        };
        static constexpr Score king_shield = {21, -11};
        static constexpr Score king_open_files = {-74, -25};
        static constexpr Score king_semi_open_files = {-59, 49};
    };
        
    inline constexpr int piece_value(const chess::PieceType type) {
        return Params::piece_type_value[static_cast<int>(type.internal())].mg;
    }

    inline uint8_t calculate_game_phase(const chess::Board& board) {
        uint8_t game_phase = 0;

        for (const chess::PieceType type : EvaluationHelper::Constants::piece_types) {
            const uint8_t phase_value = EvaluationHelper::Constants::piece_type_phase[type];

            chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
            chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);
            
            const int white_pieces = white_bb.count();
            const int black_pieces = black_bb.count();
            
            game_phase += phase_value * (white_pieces + black_pieces);
        }

        return game_phase;
    }

    inline Score evaluate_psts(const chess::Board& board, EvalTrace* trace = nullptr) {
        Score total = {0, 0};

        for (const chess::PieceType type : EvaluationHelper::Constants::piece_types) {         
            chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
            chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);
            
            while (white_bb) {
                const int pst_sq = white_bb.pop();
                total += Params::piece_type_pst[type][pst_sq];

                if (trace) {
                    trace->pst[EvalTrace::WHITE][type][pst_sq] += 1;
                }
            }

            while (black_bb) {
                const int pst_sq = black_bb.pop() ^ 56;
                total -= Params::piece_type_pst[type][pst_sq];

                if (trace) {
                    trace->pst[EvalTrace::BLACK][type][pst_sq] += 1;
                }
            }
        }

        return total;
    }

    inline Score evaluate_material(const chess::Board& board, EvalTrace* trace = nullptr) {
        Score total = {0, 0};

        for (const chess::PieceType type : EvaluationHelper::Constants::piece_types) {
            chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
            chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);

            const Score piece_value = Params::piece_type_value[type];

            const int white_pieces = white_bb.count();
            const int black_pieces = black_bb.count();

            total += piece_value * white_pieces;
            total -= piece_value * black_pieces;

            if (trace) {
                trace->material[EvalTrace::WHITE][type] += white_pieces;
                trace->material[EvalTrace::BLACK][type] += black_pieces;
            }
        }

        return total;
    }

    inline Score pawn_evaluation(const chess::Board& board, EvalTrace* trace = nullptr){
        const chess::Bitboard white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
        const chess::Bitboard black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

        const EvaluationHelper::PawnFeatures white = EvaluationHelper::get_pawn_features(white_pawns, black_pawns, chess::Color::WHITE);
        const EvaluationHelper::PawnFeatures black = EvaluationHelper::get_pawn_features(black_pawns, white_pawns, chess::Color::BLACK);

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

    inline Score mobility_evaluation(const chess::Board& board, EvalTrace* trace = nullptr) {
        Score total = {0, 0};

        const chess::Bitboard white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
        const chess::Bitboard black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

        const chess::Bitboard white_pawn_attacks = EvaluationHelper::get_all_pawn_attacks<chess::Color::WHITE>(white_pawns);
        const chess::Bitboard black_pawn_attacks = EvaluationHelper::get_all_pawn_attacks<chess::Color::BLACK>(black_pawns);

        const chess::Bitboard white_occ = board.us(chess::Color::WHITE);
        const chess::Bitboard black_occ = board.us(chess::Color::BLACK);
        
        const chess::Bitboard occ = white_occ | black_occ;

        for (const chess::PieceType type : EvaluationHelper::Constants::minor_major_pieces) {            
            chess::Bitboard white_bb = board.pieces(type, chess::Color::WHITE);
            chess::Bitboard black_bb = board.pieces(type, chess::Color::BLACK);

            while (white_bb) {
                const int sq = white_bb.pop();
                const int mobility = (EvaluationHelper::get_piece_type_attack(type, sq, occ) & ~(black_pawn_attacks | white_occ)).count();

                total += Params::piece_mobility[type] * mobility;

                if (trace) {
                    trace->piece_mobility[EvalTrace::WHITE][type] += mobility;
                }
            }

            while (black_bb) {
                const int sq = black_bb.pop();
                const int mobility = (EvaluationHelper::get_piece_type_attack(type, sq, occ) & ~(white_pawn_attacks | black_occ)).count();

                total -= Params::piece_mobility[type] * mobility;

                if (trace) {
                    trace->piece_mobility[EvalTrace::BLACK][type] += mobility;
                }
            }
        }
        

        return total;
    }

    inline Score rook_evaluation(const chess::Board& board, EvalTrace* trace = nullptr) {
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

    inline Score king_evaluation(const chess::Board& board, EvalTrace* trace = nullptr) {
        Score total = {0, 0};

        const EvaluationHelper::KingFeatures white_king_features = EvaluationHelper::get_king_features(board, chess::Color::WHITE);
        const EvaluationHelper::KingFeatures black_king_features = EvaluationHelper::get_king_features(board, chess::Color::BLACK);

        const int white_danger_idx = std::min(white_king_features.enemy_attacking_power, 15);
        const int black_danger_idx = std::min(black_king_features.enemy_attacking_power, 15);

        total += Params::king_danger[white_danger_idx] - Params::king_danger[black_danger_idx];
        total += Params::king_shield * (white_king_features.pawn_shield - black_king_features.pawn_shield);
        total += Params::king_open_files * (white_king_features.open_king_files - black_king_features.open_king_files);
        total += Params::king_semi_open_files * (white_king_features.semi_open_king_files - black_king_features.semi_open_king_files);

        if (trace) {
            EvalTrace::trace_scalar(trace->king_danger, white_king_features.enemy_attacking_power, black_king_features.enemy_attacking_power);
            EvalTrace::trace_scalar(trace->king_shield, white_king_features.pawn_shield, black_king_features.pawn_shield);
            EvalTrace::trace_scalar(trace->king_open_files, white_king_features.open_king_files, black_king_features.open_king_files);
            EvalTrace::trace_scalar(trace->king_semi_open_files, white_king_features.semi_open_king_files, black_king_features.semi_open_king_files);
        }

        return total;
    }

    int evaluation(const chess::Board& board);
}
#endif