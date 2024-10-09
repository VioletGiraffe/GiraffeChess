#include "uci.h"
#include "move.h"
#include "board.h"

#include "threading/thread_helpers.h"

#include <assert.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string_view>
#include <thread>

template <typename... Ts>
inline void reply(Ts &&...args)
{
	(std::cout << ... << args) << std::endl;
}

static void FATAL(std::string_view message)
{
	reply("info string ", message);
	abort();
}

UciServer::UciServer()
{
}

void UciServer::run()
{
	uci_loop();
}

static void uci_send_id()
{
	reply("id name GiraffeChess");
	reply("id author Violet Giraffe");
	reply("uciok");
}

inline constexpr uint8_t parseSquare(std::string_view square)
{
	uint8_t file = square[0] - 'a'; // File 'a' = 0, 'b' = 1, etc.
	uint8_t rank = square[1] - '1'; // Rank '1' = 0, '2' = 1, etc.
	return rank * 8 + file; // Convert to a 0-63 index
}

static void parseFENBoard(const std::string& fen, Board& board)
{
	static constexpr auto pieceFromLetter = [](char letter) -> PieceType {
		letter |= 0x20; // Convert to lowercase
		switch (letter)
		{
			case 'p': return PieceType::Pawn;
			case 'n': return PieceType::Knight;
			case 'b': return PieceType::Bishop;
			case 'r': return PieceType::Rook;
			case 'q': return PieceType::Queen;
			case 'k': return PieceType::King;
			default:
				assert(false);
				return PieceType::EmptySquare;
		}
	};

	static constexpr auto colorFromLetter = [](char letter) -> Color {
		return (letter & 0x20) == 0 /* upper case */ ? Color::White : Color::Black;
	};

	uint8_t row = 0, col = 0;
	board.clear();

	for (char c : fen)
	{
		if (c == '/')
		{
			// Move to the next rank
			++row;
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
			board.set(row, col, { pieceFromLetter(c), colorFromLetter(c) });
			++col;
		}
	}
}

inline constexpr uint8_t parseCastlingRights(std::string_view castling)
{
	uint8_t rights = None;

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


static void parseFEN(std::istringstream& iss, Board& board)
{
	std::string token;

	// Tokenize the FEN string
	// TODO: array, avoid heap allocation
	std::vector<std::string> components;
	components.reserve(6);

	while (std::getline(iss, token, ' '))
	{
		components.push_back(token);
	}

	// Check if the FEN string has the correct number of components
	if (components.size() != 6)
	{
		std::string fen;
		for (const auto& component : components)
			fen += component + ' ';

		FATAL("Invalid FEN string: " + fen);
		return;
	}

	parseFENBoard(components[0], board);

	// Extract and process each component
	const std::string& activeColor = components[1];
	board.setSideToMove(activeColor == "w" ? Color::White : Color::Black);

	const std::string& castlingAvailability = components[2];
	board.setCastlingRights(parseCastlingRights(castlingAvailability));

	const std::string& enPassantSquare = components[3];
	board.setEnPassantSquare(parseSquare(enPassantSquare));

	const int halfmoveClock = std::stoi(components[4]);
	const int fullmoveNumber = std::stoi(components[5]);
}

inline constexpr PieceType parsePromotion(char promotionChar)
{
	switch (promotionChar)
	{
	case 'q':
		return PieceType::Queen;
	case 'r':
		return PieceType::Rook;
	case 'b':
		return PieceType::Bishop;
	case 'n':
		return PieceType::Knight;
	default:
		assert(false);
		return PieceType::EmptySquare; // Default for non-promotion moves
	}
}

Move parseMove(const std::string &moveStr, const Board &board)
{
	const uint8_t from = parseSquare(moveStr.substr(0, 2));
	const uint8_t to = parseSquare(moveStr.substr(2, 2));

	PieceType promotion = EmptySquare;
	if (moveStr.size() == 5)
	{
		promotion = parsePromotion(moveStr[4]);
	}

	// Determine if the move is a capture (based on the board state)
	const bool isCapture = board.isEnemyPiece(to, board.pieceAt(from).color());

	return Move(from, to, isCapture, promotion);
}

static void parsePosition(std::istringstream &iss, Board& board)
{
	board.setToStartingPosition();

	std::string positionType;
	iss >> positionType;

	if (positionType == "startpos")
	{
		board.setToStartingPosition();
	}
	else if (positionType == "fen")
	{
		parseFEN(iss, board);

		// TODO:
		// Set up the board from the provided FEN string
	}
	else
	{
		std::cerr << "Invalid position type " << positionType;
		return;
	}

	// Check if there are additional moves provided
	std::string token;
	iss >> token;

	if (token == "moves")
	{
		// Parse and apply additional moves
		std::string moveString;
		while (iss >> moveString)
		{
			Move m = parseMove(moveString, board);
			if (!board.applyMove(m))
				FATAL("Invalid move: " + moveString);
		}
	}
}

void UciServer::uci_loop()
{
	Analyzer analyzer;

	std::string command;
	while (std::getline(std::cin, command))
	{
		std::istringstream is(command);

		std::string token;
		is >> std::skipws >> token;

		if (token == "stop")
		{
			analyzer.stop();
		}
		else if (token == "quit")
		{
			analyzer.stop();
			break;
		}
		else if (token == "isready")
		{
			reply("readyok");
		}
		else if (token == "ucinewgame")
		{
			analyzer.stop();

			Board board;
			board.setToStartingPosition();
			analyzer.setInitialPosition(board);
		}
		else if (token == "uci")
		{
			uci_send_id();
		}
		else if (token == "position")
		{
			Board board;
			parsePosition(is, board);
			analyzer.setInitialPosition(board);

			continue;
		}
		else if (token == "go")
		{
			is >> std::skipws >> token;

			while (token != "none")
			{
				if (token == "infinite")
				{
					// TOOO:
					break;
				}
				if (token == "movestogo")
				{
					is >> std::skipws >> token;
					// TOOO:
					is >> std::skipws >> token;
					continue;
				}

				// Depth
				if (token == "depth")
				{
					is >> std::skipws >> token;
					const int depth = std::stoi(token);
					// TODO:
					is >> std::skipws >> token;
					continue;
				}

				// Time
				if (token == "wtime")
				{
					is >> std::skipws >> token;
					const auto time = std::stod(token);
					// TODO:
					is >> std::skipws >> token;
					continue;
				}
				if (token == "btime")
				{
					is >> std::skipws >> token;
					const auto time = std::stod(token);
					// TODO:
					is >> std::skipws >> token;
					continue;
				}

				// Increment
				if (token == "winc")
				{
					is >> std::skipws >> token;
					const auto time = std::stod(token);
					// TODO:
					is >> std::skipws >> token;
					continue;
				}
				if (token == "binc")
				{
					is >> std::skipws >> token;
					const auto time = std::stod(token);
					// TODO:
					is >> std::skipws >> token;
					continue;
				}

				if (token == "movetime")
				{
					is >> std::skipws >> token;
					const auto time = std::stod(token);
					// TODO:
					is >> std::skipws >> token;
					continue;
				}
				if (token == "nodes")
				{
					is >> std::skipws >> token;
					const int nodes = stoi(token);
					// TODO:
					is >> std::skipws >> token;
				}
				token = "none";
			}

			analyzer.start();
		}

		else if (token == "setoption")
		{
			is >> std::skipws >> token;
			is >> std::skipws >> token;
		}
	}

	std::cout << std::endl;
}
