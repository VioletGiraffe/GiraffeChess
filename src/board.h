#pragma once

#include "move.h"
#include "piece.h"

#include <array>

struct MoveList
{
	inline void clear() noexcept {
		count = 0;
		::memset(moves.data(), 0, sizeof(Move) * moves.size());
	}

	template <typename... Args>
	inline void emplace_back(Args&&... args) noexcept {
		moves[count++] = { std::forward<Args>(args)... };
	}

	std::array<Move, 218> moves; // 218 is the maximum number of moves possible in any one position
	uint8_t count = 0;
};

class Board
{
public:
	void setToStartingPosition() noexcept;

	void generateMoves(Color side, MoveList& moves) const noexcept;

	// Returns false if the move is illegal (the moving piece is pinned)
	[[nodiscard]] bool applyMove(const Move& move) noexcept;

	[[nodiscard]] bool isInCheck(Color side) const noexcept;

	[[nodiscard]] Piece pieceAt(uint8_t square) const noexcept;

	[[nodiscard]] bool isEmptySquare(int rank, int file) const noexcept;
	[[nodiscard]] bool isEnemyPiece(int rank, int file, Color mySide) const noexcept;
	[[nodiscard]] bool isEnemyPiece(uint8_t square, Color mySide) const noexcept;

private:
	void generatePawnMoves(uint8_t square, MoveList&moves) const noexcept;
	void generateKnightMoves(uint8_t square, MoveList& moves) const noexcept;
	void generateBishopMoves(uint8_t square, MoveList& moves) const noexcept;
	void generateRookMoves(uint8_t square, MoveList& moves) const noexcept;
	void generateQueenMoves(uint8_t square, MoveList& moves) const noexcept;
	void generateKingMoves(uint8_t square, MoveList& moves) const noexcept;

private:
	// Row-wise. 0..7 is rank 1, 8..15 is rank 2 and so on
	std::array<Piece, 64> _squares;
	uint8_t _enPassantSquare = 0;
};
