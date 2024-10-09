#include "board.h"
#include "move_patterns.h"

#include <ranges>
#include <stddef.h>

inline constexpr uint8_t toSquare(int rank, int file) noexcept
{
	return static_cast<uint8_t>(rank * 8 + file);
}

inline constexpr bool isValidSquare(int rank, int file)
{
	return rank < 8 && file < 8 && rank >= 0 && file >= 0;
}

void Board::setToStartingPosition() noexcept
{
	// Set up the initial piece arrangement on the board
	// Assuming White pieces are in the lower ranks and Black pieces in the upper ranks

	// Pawns
	for (int file = 0; file < 8; ++file)
	{
		_squares[toSquare(1, file)] = Piece(PieceType::Pawn, Color::White);
		_squares[toSquare(6, file)] = Piece(PieceType::Pawn, Color::Black);
	}

	// Rooks
	_squares[toSquare(0, 0)] = Piece(PieceType::Rook, Color::White);
	_squares[toSquare(0, 7)] = Piece(PieceType::Rook, Color::White);
	_squares[toSquare(7, 0)] = Piece(PieceType::Rook, Color::Black);
	_squares[toSquare(7, 7)] = Piece(PieceType::Rook, Color::Black);

	// Knights
	_squares[toSquare(0, 1)] = Piece(PieceType::Knight, Color::White);
	_squares[toSquare(0, 6)] = Piece(PieceType::Knight, Color::White);
	_squares[toSquare(7, 1)] = Piece(PieceType::Knight, Color::Black);
	_squares[toSquare(7, 6)] = Piece(PieceType::Knight, Color::Black);

	// Bishops
	_squares[toSquare(0, 2)] = Piece(PieceType::Bishop, Color::White);
	_squares[toSquare(0, 5)] = Piece(PieceType::Bishop, Color::White);
	_squares[toSquare(7, 2)] = Piece(PieceType::Bishop, Color::Black);
	_squares[toSquare(7, 5)] = Piece(PieceType::Bishop, Color::Black);

	// Queens
	_squares[toSquare(0, 3)] = Piece(PieceType::Queen, Color::White);
	_squares[toSquare(7, 3)] = Piece(PieceType::Queen, Color::Black);

	// Kings
	_squares[toSquare(0, 4)] = Piece(PieceType::King, Color::White);
	_squares[toSquare(7, 4)] = Piece(PieceType::King, Color::Black);
}

void Board::clear() noexcept
{
	_squares.fill(Piece{});
	_enPassantSquare = 255;
	_sideToMove = Color::White;
	_castlingRights = WhiteKingSide | WhiteQueenSide | BlackKingSide | BlackQueenSide;
}

void Board::generateMoves(Color side, MoveList& moves) const noexcept
{
	for (uint8_t i = 0; i < 64; ++i)
	{
		const auto type = _squares[i].type();
		if (type == EmptySquare || _squares[i].color() != side)
			continue;

		switch (type)
		{
		case Pawn:
			generatePawnMoves(i, moves);
			break;
		case Knight:
			generateKnightMoves(i, moves);
			break;
		case Bishop:
			generateBishopMoves(i, moves);
			break;
		case Rook:
			generateRookMoves(i, moves);
			break;
		case Queen:
			generateQueenMoves(i, moves);
			break;
		case King:
			generateKingMoves(i, moves);
			break;
		default:
			break;
		}
	}
}

void Board::set(uint8_t rank, uint8_t file, Piece piece) noexcept
{
	_squares[toSquare(rank, file)] = piece;
}

void Board::setEnPassantSquare(uint8_t square) noexcept
{
	_enPassantSquare = square;
}

void Board::setSideToMove(Color side) noexcept
{
	_sideToMove = side;
}

void Board::setCastlingRights(uint8_t rights) noexcept
{
	_castlingRights = rights;
}

// Returns false if the move is illegal (the moving piece is pinned)
bool Board::applyMove(const Move &move) noexcept
{
	const Piece movingPiece = _squares[move.from()];
	const Piece targetPiece = _squares[move.to()];

	_squares[move.from()] = Piece{};
	_squares[move.to()] = movingPiece;

	if (isInCheck(movingPiece.color())) [[unlikely]]
	{
		// Revert
		_squares[move.from()] = movingPiece;
		_squares[move.to()] = targetPiece;
		return false;
	}

	return true;
}

Piece Board::pieceAt(uint8_t square) const noexcept
{
	return _squares[square];
}

bool Board::isEmptySquare(int rank, int file) const noexcept
{
	return pieceAt(toSquare(rank, file)).type() == EmptySquare;
}

bool Board::isEnemyPiece(int rank, int file, Color mySide) const noexcept
{
	return isEnemyPiece(toSquare(rank, file), mySide);
}

bool Board::isEnemyPiece(uint8_t square, Color mySide) const noexcept
{
	const auto piece = pieceAt(square);
	return piece.type() != EmptySquare && piece.color() != mySide;
}


void Board::generatePawnMoves(uint8_t square, MoveList &moves) const noexcept
{
	const Color side = _squares[square].color();
	const int rank = square / 8;
	const int file = square % 8;

	// Pawn moves
	const int advance = (side == White) ? 1 : -1;
	if (isValidSquare(rank + advance, file) && isEmptySquare(rank + advance, file))
	{
		moves.emplace_back(square, toSquare(rank + advance, file));
	}

	// Double pawn push
	const int doubleForward = (side == White && rank == 1) || (side == Black && rank == 6) ? advance * 2 : 0;
	if (doubleForward != 0 && isEmptySquare(rank + doubleForward, file) && isEmptySquare(rank + advance, file))
	{
		moves.emplace_back(square, toSquare(rank + doubleForward, file));
	}

	// Pawn captures
	const int leftCapture = file - 1;
	const int rightCapture = file + 1;

	if (isValidSquare(rank + advance, leftCapture) && isEnemyPiece(rank + advance, leftCapture, side))
	{
		moves.emplace_back(square, toSquare(rank + advance, leftCapture), true);
	}

	if (isValidSquare(rank + advance, rightCapture) && isEnemyPiece(rank + advance, rightCapture, side))
	{
		moves.emplace_back(square, toSquare(rank + advance, rightCapture), true);
	}

	if (_enPassantSquare != 0)
	{
		const int enPassantRank = _enPassantSquare / 8;
		const int enPassantFile = _enPassantSquare % 8;

		if ((rank == enPassantRank - advance) && (file == enPassantFile - 1 || file == enPassantFile + 1))
		{
			moves.emplace_back(square, _enPassantSquare, true);
		}
	}
}

void Board::generateKnightMoves(uint8_t square, MoveList &moves) const noexcept
{
	const Color side = _squares[square].color();
	const int rank = square / 8;
	const int file = square % 8;

	for (size_t i = 0; i < 8; ++i)
	{
		const int targetRank = rank + knightMoves[i][0];
		const int targetFile = file + knightMoves[i][1];

		if (isValidSquare(targetRank, targetFile))
		{
			const bool capture = isEnemyPiece(targetRank, targetFile, side);
			if (capture || isEmptySquare(targetRank, targetFile))
			{
				moves.emplace_back(square, toSquare(targetRank, targetFile), capture);
			}
		}
	}
}

void Board::generateBishopMoves(uint8_t square, MoveList &moves) const noexcept
{
	const Color side = _squares[square].color();
	const int rank = square / 8;
	const int file = square % 8;

	for (size_t i = 0; i < 4; ++i)
	{
		for (int j = 1; j < 8; ++j)
		{
			const int targetRank = rank + j * bishopMoveVectors[i][0];
			const int targetFile = file + j * bishopMoveVectors[i][1];

			if (!isValidSquare(targetRank, targetFile))
				break; // Stop if the target square is out of bounds

			if (isEmptySquare(targetRank, targetFile))
				moves.emplace_back(square, toSquare(targetRank, targetFile));
			else
			{
				if (isEnemyPiece(targetRank, targetFile, side))
				{
					moves.emplace_back(square, toSquare(targetRank, targetFile), true);
				}

				break; // Stop if a piece is encountered (captures beyond it are not possible)
			}
		}
	}
}

void Board::generateRookMoves(uint8_t square, MoveList &moves) const noexcept
{
	const Color side = _squares[square].color();
	const int rank = square / 8;
	const int file = square % 8;

	for (size_t i = 0; i < 4; ++i)
	{
		for (int j = 1; j < 8; ++j)
		{
			const int targetRank = rank + j * rookMoveVectors[i][0];
			const int targetFile = file + j * rookMoveVectors[i][1];

			if (!isValidSquare(targetRank, targetFile))
				break; // Stop if the target square is out of bounds

			if (isEmptySquare(targetRank, targetFile))
				moves.emplace_back(square, toSquare(targetRank, targetFile));
			else
			{
				if (isEnemyPiece(targetRank, targetFile, side))
				{
					moves.emplace_back(square, toSquare(targetRank, targetFile), true);
				}

				break; // Stop if a piece is encountered (captures beyond it are not possible)
			}
		}
	}
}

void Board::generateQueenMoves(uint8_t square, MoveList &moves) const noexcept
{
	generateRookMoves(square, moves);
	generateBishopMoves(square, moves);
}

void Board::generateKingMoves(uint8_t square, MoveList &moves) const noexcept
{
	const Color side = _squares[square].color();
	const int rank = square / 8;
	const int file = square % 8;

	const auto generateMoves = [=](auto movePattern, auto &moves)
	{
		const int targetRank = rank + movePattern[0];
		const int targetFile = file + movePattern[1];

		if (isValidSquare(targetRank, targetFile))
		{
			const bool isCapture = isEnemyPiece(targetRank, targetFile, side);
			if (isCapture || isEmptySquare(targetRank, targetFile))
			{
				moves.emplace_back(square, toSquare(targetRank, targetFile), isCapture);
			}
		}
	};

	for (const auto move : bishopMoveVectors)
		generateMoves(move, moves);

	for (const auto move : rookMoveVectors)
		generateMoves(move, moves);
}

bool Board::isInCheck(const Color side) const noexcept
{
	// Find the king's position for the specified side
	int kingRank = 0, kingFile = 0;
	for (int i = 0; i < std::size(_squares); ++i)
	{
		if (_squares[i].type() == King && _squares[i].color() == side)
		{
			kingRank = i / 8;
			kingFile = i % 8;
			break;
		}
	}

	// Check for pawn attacks
	for (const auto move : pawnAttackVectors)
	{
		const int newRank = kingRank - move[0];
		const int newFile = kingFile - move[1];
		if (isValidSquare(newRank, newFile) &&
			_squares[toSquare(newRank, newFile)].type() == PieceType::Pawn &&
			_squares[toSquare(newRank, newFile)].color() != side)
		{
			return true;
		}
	}

	for (const auto move : bishopMoveVectors)
	{
		for (int i = 1;; ++i)
		{
			const int newRank = kingRank + i * move[0];
			const int newFile = kingFile + i * move[1];
			if (!isValidSquare(newRank, newFile))
				break;

			const auto &piece = _squares[toSquare(newRank, newFile)];
			const PieceType type = piece.type();
			if ((type == PieceType::Bishop || type == PieceType::Queen) && piece.color() != side)
				return true;
			else if (type != PieceType::EmptySquare)
				break;
		}
	}

	for (const auto move : rookMoveVectors)
	{
		for (int i = 1;; ++i)
		{
			const int newRank = kingRank + i * move[0];
			const int newFile = kingFile + i * move[1];
			if (!isValidSquare(newRank, newFile))
				break;

			const auto &piece = _squares[toSquare(newRank, newFile)];
			const PieceType type = piece.type();
			if ((type == PieceType::Rook || type == PieceType::Queen) && piece.color() != side)
				return true;
			else if (type != PieceType::EmptySquare)
				break;
		}
	}

	for (const auto move : knightMoves)
	{
		const int newRank = kingRank + move[0];
		const int newFile = kingFile + move[1];
		if (isValidSquare(newRank, newFile) &&
			_squares[toSquare(newRank, newFile)].type() == PieceType::Knight &&
			_squares[toSquare(newRank, newFile)].color() != side)
		{
			return true;
		}
	}

	return false;
}
