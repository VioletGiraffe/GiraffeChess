#include "eval.h"
#include "board.h"

#include <algorithm>
#include <assert.h>

float eval(const Board& board) noexcept
{
	static constexpr auto evalPiece = [](PieceType type) noexcept {
		switch (type)
		{
		case PieceType::Pawn: return 1.0f;
		case PieceType::Knight: return 3.0f;
		case PieceType::Bishop: return 3.1f;
		case PieceType::Rook: return 5.0f;
		case PieceType::Queen: return 9.0f;
		default: return 0.0f;
		}
	};

	float score = 0.0f;

	for (uint8_t i = 0; i < 64; ++i)
	{
		const auto piece = board.pieceAt(i);
		const float multiplier = piece.color() == Color::White ? 1.0f : -1.0f;

		score += evalPiece(piece.type()) * multiplier;
	}

	return score;
}

bool isDrawPosition(const Board& board) noexcept
{
	// True if no pieces left other than kings
	for (uint8_t i = 0; i < 64; ++i)
	{
		if (board.pieceAt(i).type() != PieceType::King)
			return false;
	}

	return true;
}
