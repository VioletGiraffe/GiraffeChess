#include "perft.h"
#include "board.h"

inline constexpr uint8_t toSquare(uint8_t rank, uint8_t file) noexcept
{
	return rank * 8 + file;
}

void perft(Board& board, size_t depth, Perft& results) noexcept
{
	if (depth == 0)
	{
		results.nodes += 1;
		return;
	}

	MoveList moves;
	board.generateMoves(board.sideToMove(), moves);
	for (Move move : moves)
	{
		Board oldBoard = board;
		if (board.applyMove(move))
			perft(board, depth - 1, results);

		// Detect castling moves
		if (move.from() == toSquare(0, 4) && move.to() == toSquare(0, 6)
			|| move.from() == toSquare(7, 4) && move.to() == toSquare(7, 6)
			|| move.from() == toSquare(0, 4) && move.to() == toSquare(0, 2)
			|| move.from() == toSquare(7, 4) && move.to() == toSquare(7, 2))
		{
			results.castling += 1;
		}
		else
		{
			// Detect en passant moves
			if (board.pieceAt(move.from()).type() == Pawn && move.from() / 8 != move.to() / 8)
				results.enPassant += 1;
		}

		board = oldBoard;
	}
}
