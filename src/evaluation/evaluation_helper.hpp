#ifndef EVALUATION_HELPER_HPP
#define EVALUATION_HELPER_HPP

#include "include/chess.hpp"

namespace EvaluationHelper {
    namespace Constants {
        static constexpr uint8_t NO_PIECE_TYPES = 6;
        static constexpr uint8_t MAX_PHASE      = 24;
        static constexpr chess::Color EVAL_COLORS[2] = {chess::Color::WHITE, chess::Color::BLACK};

        static constexpr int king_attack_weight[6] = {
            0,  // Pawn
            2,  // Knight
            2,  // Bishop
            3,  // Rook
            5,  // Queen
            0   // King
        };

        static constexpr chess::PieceType piece_types[NO_PIECE_TYPES] = {
            chess::PieceType::PAWN,
            chess::PieceType::KNIGHT,
            chess::PieceType::BISHOP,
            chess::PieceType::ROOK,
            chess::PieceType::QUEEN,
            chess::PieceType::KING,
        };

        constexpr chess::PieceType minor_major_pieces[4] = {
            chess::PieceType::KNIGHT,
            chess::PieceType::BISHOP,
            chess::PieceType::ROOK,
            chess::PieceType::QUEEN
        };

        static constexpr uint8_t piece_type_phase[NO_PIECE_TYPES] = {
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

    struct PawnFeatures {
        chess::Bitboard passed;
        chess::Bitboard phalanx;

        int isolated = 0;
        int doubled = 0;
        int supported = 0;
        int backwards = 0;
    };

    struct KingFeatures {
        int pawn_shield = 0;
        int open_king_files = 0;
        int semi_open_king_files = 0;
        int enemy_attacking_power = 0;
    };

    inline int file_count(const chess::Bitboard bb) {
        // Assuming that the bb's files take up the entire file then this should count the number of files.
        static constexpr chess::Bitboard RANK_1{0xFFULL};

        return (bb & RANK_1).count();
    }

    template <chess::Color::underlying c>
    inline chess::Bitboard get_all_pawn_attacks(const chess::Bitboard pawns) {
        return chess::attacks::pawnLeftAttacks<c>(pawns) | chess::attacks::pawnRightAttacks<c>(pawns);
    }

    inline chess::Bitboard get_all_pawn_attacks(const chess::Color color, const chess::Bitboard pawns) {
        return color == chess::Color::WHITE
            ? get_all_pawn_attacks<chess::Color::WHITE>(pawns)
            : get_all_pawn_attacks<chess::Color::BLACK>(pawns);
    }

    inline chess::Bitboard get_piece_type_attack(const chess::PieceType type, const int sq, const chess::Bitboard occ) {
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

    inline chess::Bitboard get_squares_as_files(const chess::Bitboard bb) {
        return Bitboard::north_fill(bb) | Bitboard::south_fill(bb);
    }

    inline KingFeatures get_king_features(
        const chess::Board& board,
        const chess::Color color
    ) {
        KingFeatures f{};

        const bool WHITE = color == chess::Color::WHITE;

        const chess::Bitboard our_pawns = board.pieces(chess::PieceType::PAWN, color);
        const chess::Bitboard opp_pawns = board.pieces(chess::PieceType::PAWN, ~color);

        const chess::Bitboard our_king = board.pieces(chess::PieceType::KING, color);
        const chess::Bitboard king_ring = chess::attacks::king(board.kingSq(color));

        const chess::Bitboard occ = board.occ();

        const chess::Bitboard forward_king_square =
            WHITE ? (our_king << 8) : (our_king >> 8);

        const chess::Bitboard our_king_pawn_attacks =
            get_all_pawn_attacks(color, our_king);

        f.pawn_shield =
            ((our_king_pawn_attacks | forward_king_square) & our_pawns).count();

        const chess::Bitboard king_file =
            Bitboard::north_fill(Bitboard::south_fill(our_king));

        const chess::Bitboard adjacent_files =
            ((king_file & Bitboard::NOT_H_FILE) << 1) |
            ((king_file & Bitboard::NOT_A_FILE) >> 1);

        const chess::Bitboard king_files = king_file | adjacent_files;

        const chess::Bitboard our_pawn_files =
            Bitboard::north_fill(Bitboard::south_fill(our_pawns));

        const chess::Bitboard opp_pawn_files =
            Bitboard::north_fill(Bitboard::south_fill(opp_pawns));

        const chess::Bitboard all_pawn_files = our_pawn_files | opp_pawn_files;

        f.open_king_files =
            file_count(king_files & ~all_pawn_files);

        f.semi_open_king_files =
            file_count(king_files & ~our_pawn_files & opp_pawn_files);

        for (const chess::PieceType type : EvaluationHelper::Constants::minor_major_pieces) {
            chess::Bitboard opp_pieces = board.pieces(type, ~color);

            while (opp_pieces) {
                const int sq = opp_pieces.pop();
                const chess::Bitboard attacks = get_piece_type_attack(type, sq, occ);
                const chess::Bitboard king_ring_attacks = attacks & king_ring;

                if (king_ring_attacks) {
                    f.enemy_attacking_power += Constants::king_attack_weight[type];
                }
            }
        }

        return f;
    }

    inline PawnFeatures get_pawn_features(
        const chess::Bitboard our_pawns,
        const chess::Bitboard their_pawns,
        const chess::Color color
    ) {
        const bool WHITE = color == chess::Color::WHITE;

        const chess::Bitboard our_fill =
            WHITE ? Bitboard::north_fill(our_pawns) : Bitboard::south_fill(our_pawns);

        const chess::Bitboard their_fill =
            WHITE ? Bitboard::south_fill(their_pawns) : Bitboard::north_fill(their_pawns);

        const chess::Bitboard our_adjacent =
            WHITE
                ? (((our_fill & Bitboard::NOT_H_FILE) << 9) |
                  ((our_fill  & Bitboard::NOT_A_FILE) << 7))
                : (((our_fill & Bitboard::NOT_A_FILE) >> 9) |
                  ((our_fill  & Bitboard::NOT_H_FILE) >> 7));

        const chess::Bitboard their_adjacent =
            WHITE
                ? (((their_fill & Bitboard::NOT_A_FILE) >> 9) |
                  ((their_fill  & Bitboard::NOT_H_FILE) >> 7))
                : (((their_fill & Bitboard::NOT_H_FILE) << 9) |
                  ((their_fill  & Bitboard::NOT_A_FILE) << 7));

        const chess::Bitboard our_attacks   = get_all_pawn_attacks(color, our_pawns);
        const chess::Bitboard their_attacks = get_all_pawn_attacks(~color, their_pawns);

        PawnFeatures f;

        f.passed = our_pawns & ~(their_fill | their_adjacent);

        f.supported = (our_attacks & our_pawns).count();

        f.backwards =
            WHITE
                ? (our_pawns & ~our_attacks & (their_attacks >> 8)).count()
                : (our_pawns & ~our_attacks & (their_attacks << 8)).count();

        f.isolated =
            WHITE
                ? (~Bitboard::south_fill(our_adjacent) & our_pawns).count()
                : (~Bitboard::north_fill(our_adjacent) & our_pawns).count();

        f.doubled =
            WHITE
                ? ((our_fill << 8) & our_pawns).count()
                : ((our_fill >> 8) & our_pawns).count();

        f.phalanx = our_pawns & (
            ((our_pawns & Bitboard::NOT_H_FILE) << 1) |
            ((our_pawns & Bitboard::NOT_A_FILE) >> 1)
        );

        return f;
    }
}

#endif