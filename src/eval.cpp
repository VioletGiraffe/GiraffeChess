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

#ifdef _DEBUG
	//const auto blackP = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::Pawn && sq.color() == Color::Black; });
	//const auto whiteP = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::Pawn && sq.color() == Color::White; });

	//const auto blackB = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::Bishop && sq.color() == Color::Black; });
	//const auto whiteB = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::Bishop && sq.color() == Color::White; });

	//const auto blackR = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::Rook && sq.color() == Color::Black; });
	//const auto whiteR = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::Rook && sq.color() == Color::White; });

	//const auto blackQ = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::Queen && sq.color() == Color::Black; });
	//const auto whiteQ = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::Queen && sq.color() == Color::White; });

	//const auto blackN = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::Knight && sq.color() == Color::Black; });
	//const auto whiteN = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::Knight && sq.color() == Color::White; });

	//const auto blackK = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::King && sq.color() == Color::Black; });
	//const auto whiteK = std::count_if(board.squares().begin(), board.squares().end(), [](const Piece& sq) { return sq.type() == PieceType::King && sq.color() == Color::White; });
#endif

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
	// True if no piecese left other than kings
	for (uint8_t i = 0; i < 64; ++i)
	{
		if (board.pieceAt(i).type() != PieceType::King)
			return false;
	}

	return true;
}

Move findBestMove(const Board& board)
{
	MoveList moves;
	board.generateMoves(board.sideToMove(), moves);

	Move bestMove{ 0, 0, false, EmptySquare };

	float bestScore = -1e30f;
	const float multiplier = board.sideToMove() == Color::White ? 1.0f : -1.0f;

	for (auto& move : moves)
	{
		Board newBoard = board;
		if (!newBoard.applyMove(move))
			continue;

		const float score = eval(newBoard) * multiplier;
		if (score > bestScore)
		{
			bestScore = score;
			bestMove = move;
		}
	}

	return bestMove;
}
