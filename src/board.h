#pragma once

#include "move.h"
#include "piece.h"

#include <array>

enum CastlingRights : uint8_t {
	WhiteKingSide = 1,
	WhiteQueenSide = 2,
	BlackKingSide = 4,
	BlackQueenSide = 8
};

struct MoveList
{
	template <typename... Args>
	inline constexpr void emplace_back(Args&&... args) noexcept {
		_moves[_count++] = Move{ std::forward<Args>(args)... };
	}

	[[nodiscard]] inline constexpr auto begin() const noexcept {
		return _moves.begin();
	}

	[[nodiscard]] inline constexpr auto end() const noexcept {
		return _moves.begin() + _count;
	}

	[[nodiscard]] inline constexpr auto count() const noexcept {
		return _count;
	}

	[[nodiscard]] inline constexpr Move operator[](uint8_t index) const noexcept {
		return _moves[index];
	}

private:
	std::array<Move, 218> _moves; // 218 is the maximum number of moves possible in any one position
	uint8_t _count = 0;
};

class Board
{
public:
	Board& setToStartingPosition() noexcept;
	void clear() noexcept;

	void generateMoves(Color side, MoveList& moves) const noexcept;
	void set(uint8_t rank, uint8_t file, Piece piece) noexcept;
	void setEnPassantSquare(uint8_t square) noexcept;
	void setSideToMove(Color side) noexcept;
	void setCastlingRights(uint8_t rights) noexcept;

	// Returns false if the move is illegal (the moving piece is pinned)
	[[nodiscard]] bool applyMove(const Move& move) noexcept;

	[[nodiscard]] bool isInCheck(Color side) const noexcept;

	[[nodiscard]] Piece pieceAt(uint8_t square) const noexcept;
	[[nodiscard]] Piece pieceAt(int rank, int file) const noexcept;
	[[nodiscard]] inline const auto& squares() const noexcept { return _squares; }
	[[nodiscard]] Color sideToMove() const noexcept;
	[[nodiscard]] uint8_t enPassantSquare() const noexcept;
	[[nodiscard]] uint8_t castlingRights() const noexcept;

	[[nodiscard]] bool isEmptySquare(int rank, int file) const noexcept;
	[[nodiscard]] bool isEnemyPiece(int rank, int file, Color mySide) const noexcept;
	[[nodiscard]] bool isEnemyPiece(uint8_t square, Color mySide) const noexcept;

	[[nodiscard]] uint64_t hash() const noexcept;

private:
	void generatePawnMoves(uint8_t square, MoveList&moves) const noexcept;
	void generateKnightMoves(uint8_t square, MoveList& moves) const noexcept;
	void generateBishopMoves(uint8_t square, MoveList& moves) const noexcept;
	void generateRookMoves(uint8_t square, MoveList& moves) const noexcept;
	void generateQueenMoves(uint8_t square, MoveList& moves) const noexcept;
	void generateKingMoves(uint8_t square, MoveList& moves) const noexcept;
	void generateCastlingMoves(MoveList& moves, Color side) const noexcept;

	[[nodiscard]] bool isSquareAttacked(int rank, int file, Color attackingSide) const noexcept;

private:
	// Row-wise. 0..7 is rank 1, 8..15 is rank 2 and so on
	std::array<Piece, 64> _squares;
	uint8_t _enPassantSquare = 0;
	// TODO: using bitfield can save 1 byte
	Color _sideToMove       = Color::White;
	uint8_t _castlingRights = 0;
};
