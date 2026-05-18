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

    namespace Params {
        static constexpr uint8_t NO_PIECE_TYPES = 6;
        static constexpr uint8_t MAX_PHASE      = 24;

        static constexpr Score past_pawn = {100, 150};

        static constexpr chess::PieceType piece_types[NO_PIECE_TYPES] = {
            chess::PieceType::PAWN,
            chess::PieceType::KNIGHT,
            chess::PieceType::BISHOP,
            chess::PieceType::ROOK,
            chess::PieceType::QUEEN,
            chess::PieceType::KING,
        };

        static constexpr Score piece_type_value[NO_PIECE_TYPES] = {
            {100, 120},  // PAWN
            {320, 280},  // KNIGHT
            {330, 320},  // BISHOP
            {500, 560},  // ROOK
            {900, 880},  // QUEEN
            {0,   0}     // KING
        };

        static constexpr Score piece_type_pst[NO_PIECE_TYPES][64] = {
            // Pawn
            {
                { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 },
                { 98,178 }, { 134,173 }, { 61,158 }, { 95,134 }, { 68,147 }, { 126,132 }, { 34,165 }, { -11,187 },
                { -6,94 }, { 7,100 }, { 26,85 }, { 31,67 }, { 65,56 }, { 56,53 }, { 25,82 }, { -20,84 },
                { -14,32 }, { 13,24 }, { 6,13 }, { 21,5 }, { 23,-2 }, { 12,4 }, { 17,17 }, { -23,17 },
                { -27,13 }, { -2,9 }, { -5,-3 }, { 12,-7 }, { 17,-7 }, { 6,-8 }, { 10,3 }, { -25,-1 },
                { -26,4 }, { -4,7 }, { -4,-6 }, { -10,1 }, { 3,0 }, { 3,-5 }, { 33,-1 }, { -12,-8 },
                { -35,13 }, { -1,8 }, { -20,8 }, { -23,10 }, { -15,13 }, { 24,0 }, { 38,2 }, { -22,-7 },
                { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 }, { 0,0 }
            },
            // Knight
            {
                { -167,-58 }, { -89,-38 }, { -34,-13 }, { -49,-28 }, { 61,-31 }, { -97,-27 }, { -15,-63 }, { -107,-99 },
                { -73,-25 }, { -41,-8 }, { 72,-25 }, { 36,-2 }, { 23,-9 }, { 62,-25 }, { 7,-24 }, { -17,-52 },
                { -47,-24 }, { 60,-20 }, { 37,10 }, { 65,9 }, { 84,-1 }, { 129,-9 }, { 73,-19 }, { 44,-41 },
                { -9,-17 }, { 17,3 }, { 19,22 }, { 53,22 }, { 37,22 }, { 69,11 }, { 18,8 }, { 22,-18 },
                { -13,-18 }, { 4,-6 }, { 16,16 }, { 13,25 }, { 28,16 }, { 19,17 }, { 21,4 }, { -8,-18 },
                { -23,-23 }, { -9,-3 }, { 12,-1 }, { 10,15 }, { 19,10 }, { 17,-3 }, { 25,-20 }, { -16,-22 },
                { -29,-42 }, { -53,-20 }, { -12,-10 }, { -3,-5 }, { -1,-2 }, { 18,-20 }, { -14,-23 }, { -19,-44 },
                { -105,-29 }, { -21,-51 }, { -58,-23 }, { -33,-15 }, { -17,-22 }, { -28,-18 }, { -19,-50 }, { -23,-64 }
            },
            // Bishop
            {
                { -29,-14 }, { 4,-21 }, { -82,-11 }, { -37,-8 }, { -25,-7 }, { -42,-9 }, { 7,-17 }, { -8,-24 },
                { -26,-8 }, { 16,-4 }, { -18,7 }, { -13,-12 }, { 30,-3 }, { 59,-13 }, { 18,-4 }, { -47,-14 },
                { -16,2 }, { 37,-8 }, { 43,0 }, { 40,-1 }, { 35,-2 }, { 50,6 }, { 37,0 }, { -2,4 },
                { -4,-3 }, { 5,9 }, { 19,12 }, { 50,9 }, { 37,14 }, { 37,10 }, { 7,3 }, { -2,2 },
                { -6,-6 }, { 13,3 }, { 13,13 }, { 26,19 }, { 34,7 }, { 12,10 }, { 10,-3 }, { 4,-9 },
                { 0,-12 }, { 15,-3 }, { 15,8 }, { 15,10 }, { 14,13 }, { 27,3 }, { 18,-7 }, { 10,-15 },
                { 4,-14 }, { 15,-18 }, { 16,-7 }, { 0,-1 }, { 7,4 }, { 21,-9 }, { 33,-15 }, { 1,-27 },
                { -33,-23 }, { -3,-9 }, { -14,-23 }, { -21,-5 }, { -13,-9 }, { -12,-16 }, { -39,-5 }, { -21,-17 }
            },
            // Rook
            {
                { 32,13 }, { 42,10 }, { 32,18 }, { 51,15 }, { 63,12 }, { 9,12 }, { 31,8 }, { 43,5 },
                { 27,11 }, { 32,13 }, { 58,13 }, { 62,11 }, { 80,-3 }, { 67,3 }, { 26,8 }, { 44,3 },
                { -5,7 }, { 19,7 }, { 26,7 }, { 36,5 }, { 17,4 }, { 45,-3 }, { 61,-5 }, { 16,-3 },
                { -24,4 }, { -11,3 }, { 7,13 }, { 26,1 }, { 24,2 }, { 35,1 }, { -8,-1 }, { -20,2 },
                { -36,3 }, { -26,5 }, { -12,8 }, { -1,4 }, { 9,-5 }, { -7,-6 }, { 6,-8 }, { -23,-11 },
                { -45,-4 }, { -25,0 }, { -16,-5 }, { -17,-1 }, { 3,-7 }, { 0,-12 }, { -5,-8 }, { -33,-16 },
                { -44,-6 }, { -16,-6 }, { -20,0 }, { -9,2 }, { -1,-9 }, { 11,-9 }, { -6,-11 }, { -71,-3 },
                { -19,-9 }, { -13,2 }, { 1,3 }, { 17,-1 }, { 16,-5 }, { 7,-13 }, { -37,4 }, { -26,-20 }
            },
            // Queen
            {
                { -28,-9 }, { 0,22 }, { 29,22 }, { 12,27 }, { 59,27 }, { 44,19 }, { 43,10 }, { 45,20 },
                { -24,-17 }, { -39,20 }, { -5,32 }, { 1,41 }, { -16,58 }, { 57,25 }, { 28,30 }, { 54,0 },
                { -13,-20 }, { -17,6 }, { 7,9 }, { 8,49 }, { 29,47 }, { 56,35 }, { 47,19 }, { 57,9 },
                { -27,3 }, { -27,22 }, { -16,24 }, { -16,45 }, { -1,57 }, { 17,40 }, { -2,57 }, { 1,36 },
                { -9,-18 }, { -26,28 }, { -9,19 }, { -10,47 }, { -2,31 }, { -4,34 }, { 3,39 }, { -3,23 },
                { -14,-16 }, { 2,-27 }, { -11,15 }, { -2,6 }, { -5,9 }, { 2,17 }, { 14,10 }, { 5,5 },
                { -35,-22 }, { -8,-23 }, { 11,-30 }, { 2,-16 }, { 8,-16 }, { 15,-23 }, { -3,-36 }, { 1,-32 },
                { -1,-33 }, { -18,-28 }, { -9,-22 }, { 10,-43 }, { -15,-5 }, { -25,-32 }, { -31,-20 }, { -50,-41 }
            },
            // King
            {
                { -65,-74 }, { 23,-35 }, { 16,-18 }, { -15,-18 }, { -56,-11 }, { -34,15 }, { 2,4 }, { 13,-17 },
                { 29,-12 }, { -1,17 }, { -20,14 }, { -7,17 }, { -8,17 }, { -4,38 }, { -38,23 }, { -29,11 },
                { -9,10 }, { 24,17 }, { 2,23 }, { -16,15 }, { -20,20 }, { 6,45 }, { 22,44 }, { -22,13 },
                { -17,-8 }, { -20,22 }, { -12,24 }, { -27,27 }, { -30,26 }, { -25,33 }, { -14,26 }, { -36,3 },
                { -49,-18 }, { -1,-4 }, { -27,21 }, { -39,24 }, { -46,27 }, { -44,23 }, { -33,9 }, { -51,-11 },
                { -14,-19 }, { -14,-3 }, { -22,11 }, { -46,21 }, { -44,23 }, { -30,16 }, { -15,7 }, { -27,-9 },
                { 1,-27 }, { 7,-11 }, { -8,4 }, { -64,13 }, { -43,14 }, { -16,4 }, { 9,-5 }, { 8,-17 },
                { -15,-53 }, { 36,-34 }, { 12,-21 }, { -54,-11 }, { 8,-28 }, { -28,-14 }, { 24,-24 }, { 14,-43 }
            }
        };

        static constexpr int8_t piece_type_phase[NO_PIECE_TYPES] = {
            0,  // PAWN
            1,  // KNIGHT
            1,  // BISHOP
            2,  // ROOK
            4,  // QUEEN
            0   // KING
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

    inline Score pawn_evaluation(const chess::Board& board) {
        const chess::Bitboard white_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::WHITE);
        const chess::Bitboard black_pawns = board.pieces(chess::PieceType::PAWN, chess::Color::BLACK);

        // Past Pawn Evaluation is done using my PPE algorithm (https://www.reddit.com/r/chessprogramming/comments/1k416zx/ppe_parallel_pawn_evaluation_algorithm_for_hce/).
        const chess::Bitboard white_pawns_north_fill = Bitboard::north_fill(white_pawns);
        const chess::Bitboard black_pawns_south_fill = Bitboard::south_fill(black_pawns);

        const chess::Bitboard white_adjacent = ((white_pawns_north_fill & Bitboard::NOT_H_FILE) << 9) | ((white_pawns_north_fill & Bitboard::NOT_A_FILE) << 7);
        const chess::Bitboard black_adjacent = ((black_pawns_south_fill & Bitboard::NOT_A_FILE) >> 9) | ((black_pawns_south_fill & Bitboard::NOT_H_FILE) >> 7);

        // Passed pawn calculation
        const Score past_pawn_eval = 
            Params::past_pawn * (
                // White past pawns
                (~(black_pawns_south_fill | black_adjacent) & white_pawns).count() - 
                
                // Black past pawns
                (~(white_pawns_north_fill | white_adjacent) & black_pawns).count()
            );
        
        return past_pawn_eval;
    }

    int evaluation(const chess::Board& board, const int color);
}
#endif