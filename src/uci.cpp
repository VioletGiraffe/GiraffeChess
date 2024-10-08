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

UciServer::UciServer() :
	_uciThread{ &UciServer::uciThread, this }
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

void parseFEN(const std::string &fen)
{
	std::istringstream iss(fen);
	std::string token;

	// Tokenize the FEN string
	// TODO: array, avoid heap allocation
	std::vector<std::string> components;
	while (std::getline(iss, token, ' '))
	{
		components.push_back(token);
	}

	// Check if the FEN string has the correct number of components
	if (components.size() != 6)
	{
		FATAL("Invalid FEN string: " + fen);
		return;
	}

	// Extract and process each component
	const std::string& piecePlacement = components[0];
	const std::string& activeColor = components[1];
	const std::string& castlingAvailability = components[2];
	const std::string& enPassantSquare = components[3];
	const int halfmoveClock = std::stoi(components[4]);
	const int fullmoveNumber = std::stoi(components[5]);

	// TODO:

	// Process the extracted components as needed
	// Example: Print the parsed components
	std::cout << "Piece Placement: " << piecePlacement << std::endl;
	std::cout << "Active Color: " << activeColor << std::endl;
	std::cout << "Castling Availability: " << castlingAvailability << std::endl;
	std::cout << "En Passant Square: " << enPassantSquare << std::endl;
	std::cout << "Halfmove Clock: " << halfmoveClock << std::endl;
	std::cout << "Fullmove Number: " << fullmoveNumber << std::endl;
}

inline constexpr uint8_t parseSquare(std::string_view square)
{
	uint8_t file = square[0] - 'a'; // File 'a' = 0, 'b' = 1, etc.
	uint8_t rank = square[1] - '1'; // Rank '1' = 0, '2' = 1, etc.
	return rank * 8 + file; // Convert to a 0-63 index
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
		std::string fenString;
		iss >> fenString;
		parseFEN(fenString);

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
	while (!_uciThread.terminationRequested() && std::getline(std::cin, command))
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

void UciServer::uciThread() noexcept
{
	setThreadName("UCI Server");
	uci_loop();
}
