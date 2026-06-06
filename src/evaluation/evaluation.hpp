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

        int isolated_pawn[2] = {};
        int doubled_pawn[2]  = {};
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

        static constexpr Score piece_type_value[6] = {
            {135, 208}, {549, 1011}, {624, 987}, {698, 1376}, 
            {1912, 2735}, {0, 0}
        };

        static constexpr Score piece_type_pst[6][64] = {
            { {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {21, 161}, {-7, 142}, {-1, 157}, {17, 144}, {9, 175}, {101, 160}, {111, 117}, {52, 83}, {24, 147}, {3, 136}, {19, 130}, {27, 125}, {57, 145}, {63, 141}, {86, 114}, {55, 106}, {24, 169}, {12, 162}, {32, 120}, {46, 112}, {64, 113}, {68, 126}, {56, 129}, {36, 123}, {29, 219}, {32, 197}, {40, 162}, {82, 99}, {104, 114}, {125, 125}, {64, 162}, {31, 161}, {32, 306}, {68, 266}, {117, 191}, {111, 96}, {140, 84}, {268, 136}, {119, 187}, {68, 205}, {202, 346}, {180, 328}, {183, 264}, {237, 157}, {203, 146}, {124, 196}, {-211, 328}, {-172, 341}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0} },
            { {-201, -48}, {27, -28}, {-39, 77}, {26, 101}, {42, 90}, {49, 66}, {36, 4}, {-156, -9}, {-31, 61}, {4, 112}, {31, 94}, {72, 123}, {59, 139}, {59, 85}, {23, 109}, {46, 66}, {14, 15}, {52, 122}, {62, 135}, {85, 200}, {104, 188}, {81, 134}, {75, 128}, {47, 22}, {47, 102}, {57, 190}, {93, 229}, {96, 254}, {112, 251}, {98, 250}, {145, 200}, {85, 140}, {71, 114}, {93, 194}, {130, 233}, {157, 274}, {122, 281}, {173, 256}, {121, 237}, {150, 149}, {-24, 103}, {81, 170}, {146, 214}, {175, 217}, {255, 191}, {292, 219}, {175, 183}, {173, 90}, {22, 48}, {11, 128}, {112, 141}, {158, 222}, {173, 206}, {249, 93}, {0, 134}, {105, 53}, {-347, -91}, {-163, 110}, {-239, 218}, {25, 150}, {173, 136}, {-165, 225}, {-13, 112}, {-258, -133} },
            { {88, 128}, {98, 162}, {49, 141}, {19, 161}, {2, 179}, {44, 153}, {60, 155}, {72, 146}, {83, 130}, {89, 143}, {99, 155}, {60, 179}, {68, 169}, {80, 134}, {121, 151}, {91, 64}, {54, 161}, {82, 185}, {75, 203}, {87, 207}, {77, 214}, {84, 181}, {74, 169}, {92, 170}, {43, 160}, {69, 192}, {79, 213}, {113, 211}, {132, 191}, {56, 214}, {71, 191}, {78, 164}, {11, 197}, {96, 210}, {86, 199}, {176, 192}, {119, 221}, {133, 196}, {93, 236}, {66, 208}, {36, 192}, {59, 227}, {134, 185}, {105, 202}, {152, 195}, {175, 225}, {158, 217}, {102, 213}, {-64, 211}, {28, 212}, {26, 213}, {1, 231}, {32, 224}, {84, 200}, {11, 205}, {66, 172}, {-8, 221}, {-93, 257}, {-232, 263}, {-156, 273}, {-130, 241}, {-149, 247}, {122, 191}, {-6, 227} },
            { {117, 585}, {121, 598}, {132, 605}, {153, 582}, {149, 588}, {161, 609}, {194, 576}, {142, 538}, {23, 602}, {88, 589}, {91, 609}, {98, 602}, {104, 599}, {154, 576}, {172, 572}, {32, 616}, {70, 602}, {81, 638}, {66, 640}, {91, 627}, {90, 635}, {113, 632}, {200, 610}, {129, 590}, {78, 656}, {68, 695}, {74, 701}, {99, 683}, {96, 682}, {115, 689}, {177, 671}, {150, 646}, {107, 693}, {138, 699}, {155, 707}, {212, 689}, {182, 698}, {213, 685}, {260, 661}, {206, 673}, {110, 714}, {200, 692}, {196, 713}, {249, 685}, {298, 666}, {379, 674}, {458, 611}, {262, 675}, {3, 294}, {-13, 314}, {54, 301}, {111, 300}, {95, 309}, {205, 235}, {102, 265}, {159, 244}, {242, 701}, {244, 712}, {193, 728}, {223, 713}, {247, 720}, {359, 711}, {385, 703}, {388, 698} },
            { {406, 636}, {409, 619}, {428, 587}, {436, 671}, {427, 609}, {363, 649}, {423, 538}, {399, 581}, {382, 663}, {401, 677}, {422, 638}, {413, 693}, {419, 674}, {448, 569}, {465, 538}, {448, 585}, {377, 697}, {404, 736}, {402, 780}, {391, 761}, {394, 767}, {400, 817}, {432, 761}, {430, 738}, {383, 722}, {380, 800}, {383, 804}, {365, 910}, {365, 916}, {403, 926}, {416, 890}, {431, 933}, {367, 749}, {366, 836}, {350, 847}, {346, 949}, {351, 1041}, {398, 1081}, {441, 1061}, {448, 1020}, {338, 797}, {347, 830}, {328, 901}, {367, 939}, {398, 1044}, {541, 1078}, {563, 1059}, {484, 1091}, {315, 872}, {239, 954}, {323, 937}, {274, 1063}, {333, 1120}, {488, 1064}, {328, 1155}, {475, 1030}, {329, 924}, {359, 936}, {369, 986}, {443, 952}, {460, 995}, {646, 943}, {647, 941}, {612, 924} },
            { {-54, -129}, {60, -84}, {-20, -50}, {-241, -46}, {-83, -125}, {-225, -13}, {26, -94}, {40, -225}, {8, -26}, {-52, 0}, {-110, 34}, {-249, 62}, {-175, 44}, {-182, 49}, {-28, -17}, {16, -81}, {-96, -24}, {-94, 35}, {-92, 60}, {-146, 101}, {-113, 90}, {-122, 67}, {-51, 12}, {-108, -18}, {-16, -50}, {25, 46}, {80, 83}, {-92, 134}, {-29, 117}, {-32, 93}, {-6, 53}, {-183, 0}, {-59, 30}, {141, 74}, {101, 110}, {31, 117}, {-6, 124}, {101, 108}, {24, 101}, {-176, 28}, {-22, 28}, {176, 103}, {272, 78}, {139, 74}, {228, 51}, {287, 100}, {176, 122}, {-104, 22}, {30, -103}, {93, 77}, {305, 20}, {180, 15}, {107, 36}, {176, 54}, {67, 116}, {-20, -82}, {86, -432}, {138, -162}, {150, -120}, {93, -60}, {89, -97}, {94, -71}, {114, -60}, {-17, -377} }
        };

        static constexpr Score past_pawn[8] = {
            {0, 0}, {-22, 40}, {-36, 52}, {-23, 114}, 
            {43, 164}, {78, 324}, {203, 365}, {0, 0}
        };
        
        static constexpr Score piece_mobility[6] = {
            {0, 0}, {10, 9}, {10, 19}, {7, 10}, {5, 12}, {0, 0}
        
        };
        
        static constexpr Score rook_on_open_file = {87, 9};
        static constexpr Score rook_on_semi_open_file = {38, 29};
        static constexpr Score connected_rooks = {-2, 91};
        static constexpr Score rook_on_seventh = {148, 426};
        static constexpr Score isolated_pawn = {-27, -41};
        static constexpr Score doubled_pawn = {-35, -52};
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

        // Isolated pawns and Doubled Pawns
        const int white_isolated_pawns = (~Bitboard::south_fill(white_adjacent) & white_pawns).count();
        const int black_isolated_pawns = (~Bitboard::north_fill(black_adjacent) & black_pawns).count();

        const int white_doubled_pawns = ((white_pawns_north_fill << 8) & white_pawns).count();
        const int black_doubled_pawns = ((black_pawns_south_fill >> 8) & black_pawns).count();

        total += Params::isolated_pawn * (white_isolated_pawns - black_isolated_pawns);
        total += Params::doubled_pawn  * (white_doubled_pawns  - black_doubled_pawns);

        if (trace) {
            trace->isolated_pawn[EvalTrace::WHITE] += white_isolated_pawns;
            trace->isolated_pawn[EvalTrace::BLACK] += black_isolated_pawns;
            trace->doubled_pawn[EvalTrace::WHITE]  += white_doubled_pawns;
            trace->doubled_pawn[EvalTrace::BLACK]  += black_doubled_pawns;
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