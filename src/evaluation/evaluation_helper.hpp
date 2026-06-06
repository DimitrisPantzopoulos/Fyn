#ifndef EVALUATION_HELPER_HPP
#define EVALUATION_HELPER_HPP

#include "include/chess.hpp"

namespace EvaluationHelper {
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

    template <chess::Color::underlying c>
    PawnFeatures get_pawn_features(
        chess::Bitboard our_pawns,
        chess::Bitboard their_pawns
    ) {
        constexpr bool WHITE = c == chess::Color::WHITE;
        constexpr chess::Color::underlying their_color =
            c == chess::Color::WHITE ? chess::Color::BLACK : chess::Color::WHITE;

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

        const chess::Bitboard our_attacks   = get_all_pawn_attacks<c>(our_pawns);
        const chess::Bitboard their_attacks = get_all_pawn_attacks<their_color>(their_pawns);

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