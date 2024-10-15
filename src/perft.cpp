#include "perft.h"
#include "board.h"

#include <iostream>

inline constexpr uint8_t toSquare(uint8_t rank, uint8_t file) noexcept
{
	return rank * 8 + file;
}

static void perft(Board& board, size_t depth, Perft& results, const PerftPrintFunc& printFunc, bool print) noexcept
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
		const uint64_t prevNodesCount = results.nodes;

		Board oldBoard = board;
		if (board.applyMove(move))
		{
			perft(board, depth - 1, results, printFunc, false);

			// Detect castling moves
			if (board.pieceAt(move.from()).type() == King &&
				(   move.from() == toSquare(0, 4) && move.to() == toSquare(0, 6)
				 || move.from() == toSquare(7, 4) && move.to() == toSquare(7, 6)
				 || move.from() == toSquare(0, 4) && move.to() == toSquare(0, 2)
				 || move.from() == toSquare(7, 4) && move.to() == toSquare(7, 2)
				)) [[unlikely]]
			{
				results.castling += 1;
			}
			else
			{
				// Detect en passant moves
				if (oldBoard.pieceAt(move.from()).type() == Pawn && move.isCapture() && oldBoard.pieceAt(move.to()).type() == EmptySquare)
					results.enPassant += 1;

				results.captures += (uint64_t)move.isCapture();
			}

			if (print && printFunc) [[unlikely]]
				printFunc(move.notation(), results.nodes - prevNodesCount);
		}

		board = oldBoard;
	}
}

void perft(Board& board, size_t depth, Perft& results, const PerftPrintFunc& printFunc) noexcept
{
	perft(board, depth, results, printFunc, true);
}
