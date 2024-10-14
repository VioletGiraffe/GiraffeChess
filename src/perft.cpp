#include "perft.h"
#include "board.h"

uint64_t perft(Board& board, size_t depth) noexcept
{
	uint64_t nodes = 0;

	MoveList moves;
	board.generateMoves(board.sideToMove(), moves);
	for (Move move : moves)
	{
		Board oldBoard = board;
		if (!board.applyMove(move))
			continue;

		++nodes;
		if (depth > 1)
			nodes += perft(board, depth - 1);

		board = oldBoard;
	}

	return nodes;
}
