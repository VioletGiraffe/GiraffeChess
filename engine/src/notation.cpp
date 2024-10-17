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

static void parseFENBoard(const std::string& fen, Board& board)
{
	uint8_t row = 7, col = 0;
	board.clear();

	for (char c : fen)
	{
		if (c == '/')
		{
			// Move to the next rank
			--row;
			col = 0;
		}
		else if (isdigit(c))
		{
			// Empty squares, the number tells us how many
			col += c - '0';  // Convert character to digit
		}
		else
		{
			// A piece (R, N, B, Q, K, P or r, n, b, q, k, p)
			board.set(row, col, pieceFromLetter(c));
			++col;
		}
	}
}

inline constexpr uint8_t parseCastlingRights(std::string_view castling)
{
	uint8_t rights = 0;

	for (char c : castling)
	{
		switch (c) {
		case 'K':
			rights |= WhiteKingSide;
			break;
		case 'Q':
			rights |= WhiteQueenSide;
			break;
		case 'k':
			rights |= BlackKingSide;
			break;
		case 'q':
			rights |= BlackQueenSide;
			break;
		default:
			// No castling available, leave `rights` as None
			break;
		}
	}

	return rights;
}

void parseFEN(std::istringstream& iss, Board& board)
{
	// Tokenize the FEN string
	// TODO: array, avoid heap allocation
	std::array<std::string, 6> components;

	for (size_t i = 0; i < 6; ++i)
		iss >> std::skipws >> components[i];

	const std::string& initialPosition = components[0];
	parseFENBoard(initialPosition, board);

	// Extract and process each component
	const std::string& activeColor = components[1];
	board.setSideToMove(activeColor == "w" ? Color::White : Color::Black);

	const std::string& castlingAvailability = components[2];
	board.setCastlingRights(parseCastlingRights(castlingAvailability));

	const std::string& enPassantSquare = components[3];
	board.setEnPassantSquare(enPassantSquare == "-" ? 0 : parseSquare(enPassantSquare));

	//const int halfmoveClock = components[4].empty() ? 0 : std::stoi(components[4]);
	//const int fullmoveNumber =  components[5].empty() ? 1 : std::stoi(components[5]);

	[[maybe_unused]] const auto newFen = generateFEN(board);
	assert(newFen == initialPosition + " " + activeColor + " " + castlingAvailability + " " + enPassantSquare + " " + "0" + ' ' + "1");
}