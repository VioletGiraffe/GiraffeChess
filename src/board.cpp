#include "board.h"
#include "move_patterns.h"
#include "hash/wheathash.hpp"

#include <assert.h>
#include <stddef.h>

inline constexpr uint8_t toSquare(int rank, int file) noexcept
{
	return static_cast<uint8_t>(rank * 8 + file);
}

inline constexpr bool isValidSquare(int rank, int file)
{
	return rank < 8 && file < 8 && rank >= 0 && file >= 0;
}

inline constexpr Color oppositeSide(Color side) noexcept
{
	return side == Color::White ? Color::Black : Color::White;
}

static constexpr uint8_t whiteKingStart = toSquare(0, 4);  // e1
static constexpr uint8_t blackKingStart = toSquare(7, 4);  // e8
static constexpr uint8_t whiteKingsideRookStart = toSquare(0, 7);  // h1
static constexpr uint8_t whiteQueensideRookStart = toSquare(0, 0);  // a1
static constexpr uint8_t blackKingsideRookStart = toSquare(7, 7);  // h8
static constexpr uint8_t blackQueensideRookStart = toSquare(7, 0);  // a8

Board& Board::setToStartingPosition() noexcept
{
	// Set up the initial piece arrangement on the board
	// Assuming White pieces are in the lower ranks and Black pieces in the upper ranks

	_castlingRights = WhiteKingSide | WhiteQueenSide | BlackKingSide | BlackQueenSide;
	_enPassantSquare = 0;

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

	return *this;
}

void Board::clear() noexcept
{
	_squares.fill(Piece{});
	_enPassantSquare = 0;
	_sideToMove = Color::White;
	_castlingRights = 0;
}

void Board::generateMoves(Color side, MoveList& moves) const noexcept
{
	for (uint8_t i = 0; i < 64; ++i)
	{
		const Piece movingPiece = _squares[i];
		const auto type = movingPiece.type();
		if (type == EmptySquare || movingPiece.color() != side)
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

	generateCastlingMoves(moves, side);
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

	const auto to = move.to();
	_enPassantSquare = 0;

	// Handle castling moves
	if (movingPiece.type() == King)
	{
		// TODO: convert to switch

		if (move.from() == whiteKingStart && move.to() == toSquare(0, 6)) // White king side castling
		{
			// Move the rook (king will be moved by the normal path)
			_squares[toSquare(0, 5)] = Piece(PieceType::Rook, Color::White);
			_squares[toSquare(0, 7)] = Piece{};
			_castlingRights &= ~(WhiteKingSide | WhiteQueenSide);
		}
		else if (move.from() == blackKingStart && move.to() == toSquare(7, 6)) // Black king side castling
		{
			// Move the rook (king will be moved by the normal path)
			_squares[toSquare(7, 5)] = Piece(PieceType::Rook, Color::Black);
			_squares[toSquare(7, 7)] = Piece{};
			_castlingRights &= ~(BlackKingSide | BlackQueenSide);
		}
		else if (move.from() == whiteKingStart && move.to() == toSquare(0, 2)) // White queen side castling
		{
			// Move the rook (king will be moved by the normal path)
			_squares[toSquare(0, 3)] = Piece(PieceType::Rook, Color::White);
			_squares[toSquare(0, 0)] = Piece{};
			_castlingRights &= ~(WhiteKingSide | WhiteQueenSide);
		}
		else if (move.from() == blackKingStart && move.to() == toSquare(7, 2)) // Black queen side castling
		{
			// Move the rook (king will be moved by the normal path)
			_squares[toSquare(7, 3)] = Piece(PieceType::Rook, Color::Black);
			_squares[toSquare(7, 0)] = Piece{};
			_castlingRights &= ~(BlackKingSide | BlackQueenSide);
		}
	}

	_squares[move.from()] = Piece{};
	_squares[move.to()] = movingPiece;

	if (movingPiece.type() == Pawn)
	{
		// Handle en passant availability
		const int diff = (int)move.to() - (int)move.from();
		if (diff == 2 * 8 || diff == -2 * 8) // Double pawn push
		{
			_enPassantSquare = move.to() - (diff / 2); // The square behind the pawn
		}
		else if (move.isCapture() && targetPiece.type() == EmptySquare) // En passant capture - remove the captured pawn
		{
			_squares[move.to() - (diff / 2)] = Piece{};
		}
		else if (move.promotion() != EmptySquare) [[unlikely]]
		{
			// Handle promotion
			_squares[move.to()] = Piece{ move.promotion(), movingPiece.color() };
		}
	}

	if (isInCheck(movingPiece.color())) [[unlikely]]
		return false;

	_sideToMove = oppositeSide(_sideToMove);

	return true;
}

Piece Board::pieceAt(uint8_t square) const noexcept
{
	return _squares[square];
}

Color Board::sideToMove() const noexcept
{
	return _sideToMove;
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

uint64_t Board::hash() const noexcept
{
	uint64_t hash = wheathash64(_squares.data(), _squares.size() * sizeof(Piece));
	hash ^= wheathash64v(((uint64_t)_castlingRights << 16) | ((uint64_t)_enPassantSquare << 8) | (uint64_t)_sideToMove);
	return hash;
}


void Board::generatePawnMoves(uint8_t square, MoveList &moves) const noexcept
{
	// TODO: pass 'side' from the caller
	const Color side = _squares[square].color();
	const int rank = square / 8;
	const int file = square % 8;

	// Pawn push
	const int advance = (side == White) ? 1 : -1;
	const int targetRank = rank + advance;
	const int promotionRank = (side == White) ? 7 : 0;

	if (isValidSquare(targetRank, file) && isEmptySquare(targetRank, file))
	{
		if (targetRank == promotionRank) [[unlikely]]
		{
			// Generate promotion moves (Queen, Rook, Bishop, Knight)
			moves.emplace_back(square, toSquare(targetRank, file), false, Queen);
			moves.emplace_back(square, toSquare(targetRank, file), false, Rook);
			moves.emplace_back(square, toSquare(targetRank, file), false, Bishop);
			moves.emplace_back(square, toSquare(targetRank, file), false, Knight);
		}
		else
			moves.emplace_back(square, toSquare(targetRank, file));
	}

	// Double pawn push
	const int doubleForward = (side == White && rank == 1) || (side == Black && rank == 6) ? advance * 2 : 0;
	if (doubleForward != 0 && isEmptySquare(rank + doubleForward, file) && isEmptySquare(targetRank, file))
	{
		moves.emplace_back(square, toSquare(rank + doubleForward, file));
	}

	// Pawn captures
	const int leftCapture = file - 1;
	const int rightCapture = file + 1;

	for (auto captureFile : { leftCapture, rightCapture })
	{
		if (isValidSquare(targetRank, captureFile) && isEnemyPiece(targetRank, captureFile, side))
		{
			if (targetRank == promotionRank) [[unlikely]]
			{
				// Generate promotion moves (Queen, Rook, Bishop, Knight)
				moves.emplace_back(square, toSquare(targetRank, captureFile), true, Queen);
				moves.emplace_back(square, toSquare(targetRank, captureFile), true, Rook);
				moves.emplace_back(square, toSquare(targetRank, captureFile), true, Bishop);
				moves.emplace_back(square, toSquare(targetRank, captureFile), true, Knight);
			}
			else
				moves.emplace_back(square, toSquare(targetRank, captureFile), true);
		}
	}

	if (_enPassantSquare != 0)
	{
		const int enPassantRank = _enPassantSquare / 8;
		const int enPassantFile = _enPassantSquare % 8;

		if ((rank == enPassantRank - advance) && (leftCapture == enPassantFile || rightCapture == enPassantFile))
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

	const auto generateMoves = [&](auto movePattern)
	{
		const int targetRank = rank + movePattern[0];
		const int targetFile = file + movePattern[1];

		if (isValidSquare(targetRank, targetFile))
		{
			const Piece target = _squares[toSquare(targetRank, targetFile)];
			if (target.type() == EmptySquare || target.color() != side)
			{
				const bool capture = target.type() != EmptySquare;
				moves.emplace_back(square, toSquare(targetRank, targetFile), capture);
			}
		}
	};

	for (const auto move : bishopMoveVectors)
		generateMoves(move);

	for (const auto move : rookMoveVectors)
		generateMoves(move);
}

void Board::generateCastlingMoves(MoveList& moves, Color side) const noexcept
{
	if (side == White)
	{
		if ((_castlingRights & WhiteKingSide) && _squares[whiteKingStart] == Piece{PieceType::King, Color::White} && _squares[whiteKingsideRookStart] == Piece{PieceType::Rook, Color::White})
		{
			// Check if squares f1 and g1 are empty and the king isn't in check
			if (isEmptySquare(0, 5) && isEmptySquare(0, 6) &&
				!isSquareAttacked(0, 4, Black) && !isSquareAttacked(0, 5, Black) && !isSquareAttacked(0, 6, Black))
			{
				moves.emplace_back(whiteKingStart, toSquare(0, 6));  // Kingside castling
			}
		}

		if (_castlingRights & WhiteQueenSide && _squares[whiteKingStart] == Piece{ PieceType::King, Color::White } && _squares[whiteQueensideRookStart] == Piece{ PieceType::Rook, Color::White })
		{
			// Check if squares b1, c1, and d1 are empty and the king isn't in check
			if (isEmptySquare(0, 1) && isEmptySquare(0, 2) && isEmptySquare(0, 3) &&
				!isSquareAttacked(0, 4, Black) && !isSquareAttacked(0, 3, Black) && !isSquareAttacked(0, 2, Black))
			{
				moves.emplace_back(whiteKingStart, toSquare(0, 2));  // Queenside castling
			}
		}
	}
	else
	{
		if (_castlingRights & BlackKingSide && _squares[blackKingStart] == Piece{ PieceType::King, Color::Black } && _squares[blackKingsideRookStart] == Piece{ PieceType::Rook, Color::Black })
		{
			// Check if squares f8 and g8 are empty and the king isn't in check
			if (isEmptySquare(7, 5) && isEmptySquare(7, 6) &&
				!isSquareAttacked(7, 4, White) && !isSquareAttacked(7, 5, White) && !isSquareAttacked(7, 6, White))
			{
				moves.emplace_back(blackKingStart, toSquare(7, 6));  // Kingside castling
			}
		}

		if (_castlingRights & BlackQueenSide && _squares[blackKingStart] == Piece{ PieceType::King, Color::Black } && _squares[blackQueensideRookStart] == Piece{ PieceType::Rook, Color::Black })
		{
			// Check if squares b8, c8, and d8 are empty and the king isn't in check
			if (isEmptySquare(7, 1) && isEmptySquare(7, 2) && isEmptySquare(7, 3) &&
				!isSquareAttacked(7, 4, White) && !isSquareAttacked(7, 3, White) && !isSquareAttacked(7, 2, White))
			{
				moves.emplace_back(blackKingStart, toSquare(7, 2));  // Queenside castling
			}
		}
	}
}

bool Board::isSquareAttacked(int rank, int file, Color attackingSide) const noexcept
{
	// Precompute common piece offsets
	static constexpr int knightOffsets[8][2] = { {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
											{1, -2}, {1, 2}, {2, -1}, {2, 1} };
	static constexpr int kingOffsets[8][2] = { {-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
										  {0, 1}, {1, -1}, {1, 0}, {1, 1} };

	// Check for pawn attacks
	const int pawnAdvance = (attackingSide == White) ? -1 : 1;
	if (isValidSquare(rank + pawnAdvance, file - 1) && _squares[toSquare(rank + pawnAdvance, file - 1)].type() == Pawn &&
		_squares[toSquare(rank + pawnAdvance, file - 1)].color() == attackingSide)
		return true;

	if (isValidSquare(rank + pawnAdvance, file + 1) && _squares[toSquare(rank + pawnAdvance, file + 1)].type() == Pawn &&
		_squares[toSquare(rank + pawnAdvance, file + 1)].color() == attackingSide)
		return true;

	// Check for knight attacks
	for (const auto& offset : knightOffsets)
	{
		int targetRank = rank + offset[0];
		int targetFile = file + offset[1];
		if (isValidSquare(targetRank, targetFile) &&
			_squares[toSquare(targetRank, targetFile)].type() == Knight &&
			_squares[toSquare(targetRank, targetFile)].color() == attackingSide)
			return true;
	}

	// Check for king attacks
	for (const auto& offset : kingOffsets)
	{
		int targetRank = rank + offset[0];
		int targetFile = file + offset[1];
		if (isValidSquare(targetRank, targetFile) &&
			_squares[toSquare(targetRank, targetFile)].type() == King &&
			_squares[toSquare(targetRank, targetFile)].color() == attackingSide)
			return true;
	}

	// General sliding attacks (bishop/rook/queen)
	// Combine diagonal (bishop/queen) and straight-line (rook/queen) into a single sliding loop
	static constexpr std::pair<int, int> directions[] {
		{-1, -1}, {-1, 1}, {1, -1}, {1, 1},  // Diagonal directions
		{-1, 0}, {1, 0}, {0, -1}, {0, 1}     // Straight-line directions
	};

	for (const auto& dir : directions)
	{
		int targetRank = rank + dir.first;
		int targetFile = file + dir.second;
		while (isValidSquare(targetRank, targetFile))
		{
			const auto piece = _squares[toSquare(targetRank, targetFile)];
			const auto pieceType = piece.type();
			const auto pieceColor = piece.color();

			if (pieceType != EmptySquare)
			{
				if (pieceColor == attackingSide &&
					((pieceType == Bishop && (dir.first != 0 && dir.second != 0)) ||  // Diagonal attack (bishop)
					 (pieceType == Rook && (dir.first == 0 || dir.second == 0))   ||   // Straight-line attack (rook)
					  pieceType == Queen))                                           // Queen can attack both diagonally and straight
				{
					return true;
				}

				break;  // Stop further sliding in this direction if blocked
			}

			targetRank += dir.first;
			targetFile += dir.second;
		}
	}

	// No attack detected
	return false;
}

bool Board::isInCheck(const Color side) const noexcept
{
	// Find the king's position for the specified side
	int kingRank = 0, kingFile = 0;
	for (uint32_t i = 0; i < 64; ++i)
	{
		if (_squares[i] == Piece{ King, side })
		{
			kingRank = i / 8;
			kingFile = i % 8;
			break;
		}
	}

	// Check for pawn attacks
	for (const auto move : pawnAttackVectors)
	{
		const auto attackerSide = oppositeSide(side);
		const int newRank = kingRank - (move[0] * attackerSide == White ? 1 : -1);
		const int newFile = kingFile - move[1];
		if (isValidSquare(newRank, newFile) &&
			_squares[toSquare(newRank, newFile)] == Piece{ Pawn, attackerSide })
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

			const auto piece = _squares[toSquare(newRank, newFile)];
			const PieceType type = piece.type();
			if ((type == PieceType::Bishop || type == PieceType::Queen) && piece.color() != side)
				return true;
			else if (i == 1 && piece == Piece{ King, oppositeSide(side) })
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

			const auto piece = _squares[toSquare(newRank, newFile)];
			const PieceType type = piece.type();
			if ((type == PieceType::Rook || type == PieceType::Queen) && piece.color() != side)
				return true;
			else if (i == 1 && piece == Piece{ King, oppositeSide(side) })
				return true;
			else if (type != PieceType::EmptySquare)
				break;
		}
	}

	for (const auto move : knightMoves)
	{
		const int newRank = kingRank + move[0];
		const int newFile = kingFile + move[1];
		if (isValidSquare(newRank, newFile) && _squares[toSquare(newRank, newFile)] == Piece{ Knight, oppositeSide(side) })
			return true;
	}

	return false;
}
