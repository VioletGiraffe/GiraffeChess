#include "notation.h"

#include "board.h"

std::string generateFEN(const Board& board)
{
	std::string fen;

	// Piece Placement
	for (int rank = 7; rank >= 0; --rank)
	{
		int emptyCount = 0;
		for (int file = 0; file < 8; ++file)
		{
			const Piece piece = board.pieceAt(rank, file);
			if (piece.type() == PieceType::EmptySquare)
			{
				emptyCount++;
			}
			else
			{
				if (emptyCount > 0)
				{
					fen += std::to_string(emptyCount);
					emptyCount = 0;
				}

				fen += piece.notation();
			}
		}

		// Handle empty squares at the end of the rank
		if (emptyCount > 0)
		{
			fen += std::to_string(emptyCount);
		}

		if (rank > 0)
		{
			fen += '/';
		}
	}

	// Side to Move
	fen += ' ';
	fen += (board.sideToMove() == White) ? 'w' : 'b';

	// Castling Rights
	fen += ' ';
	const auto castlingRights = board.castlingRights();
	const bool castlingAvailable = castlingRights != 0;
	if (castlingRights & WhiteKingSide)
		fen += 'K';
	if (castlingRights & WhiteQueenSide)
		fen += 'Q';
	if (castlingRights & BlackKingSide)
		fen += 'k';
	if (castlingRights & BlackQueenSide)
		fen += 'q';

	if (!castlingAvailable)
		fen += '-';

	// En Passant Target Square
	fen += ' ';
	if (board.enPassantSquare() != 0)
		fen += indexToSquareNotation(board.enPassantSquare());
	else
		fen += '-';

	// Halfmove Clock
	fen += ' ';
	fen += '0';

	// Fullmove Number
	fen += ' ';
	fen += '1';

	return fen;
}
