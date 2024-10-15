#include "uci.h"
#include "move.h"
#include "board.h"
#include "debug.h"
#include "logger.h"
#include "perft.h"
#include "notation.h"

#include "system/ctimeelapsed.h"

#include <algorithm>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <string_view>

template <typename... Ts>
inline void reply(Ts &&...args)
{
	log("response: ", args...);
	(std::cout << ... << args) << std::endl;
}

[[noreturn]] static void FATAL(std::string_view message)
{
	reply("info string ", message);
	log(message);
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


static void parseFEN(std::istringstream& iss, Board& board)
{
	// Tokenize the FEN string
	// TODO: array, avoid heap allocation
	std::array<std::string, 6> components;

	for (size_t i = 0; i < 6; ++i)
		iss >> std::skipws >> components[i];

	parseFENBoard(components[0], board);

	// Extract and process each component
	const std::string& activeColor = components[1];
	board.setSideToMove(activeColor == "w" ? Color::White : Color::Black);

	const std::string& castlingAvailability = components[2];
	board.setCastlingRights(parseCastlingRights(castlingAvailability));

	const std::string& enPassantSquare = components[3];
	board.setEnPassantSquare(enPassantSquare == "-" ? 0 : parseSquare(enPassantSquare));

	//const int halfmoveClock = components[4].empty() ? 0 : std::stoi(components[4]);
	//const int fullmoveNumber =  components[5].empty() ? 1 : std::stoi(components[5]);

	assert(generateFEN(board) == components[0] + " " + components[1] + " " + components[2] + " " + components[3] + " " + "0" + ' ' + "1");
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
	iss >> std::skipws >> positionType;

	if (positionType == "startpos")
	{
		board.setToStartingPosition();
	}
	else if (positionType == "fen")
	{
		parseFEN(iss, board);
	}
	else
	{
		std::cerr << "Invalid position type " << positionType;
		return;
	}

	// Check if there are additional moves provided
	std::string token;
	iss >> std::skipws >> token;

	if (token == "moves")
	{
		// Parse and apply additional moves
		std::string moveString;
		while (iss >> std::skipws >> moveString)
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
	analyzer.setInitialPosition(Board{}.setToStartingPosition());

	std::string command;
	while (std::getline(std::cin, command))
	{
		log(command);

		std::istringstream is(command);

		std::string token;
		is >> std::skipws >> token;

		if (token == "stop")
		{
			analyzer.stop();
		}
		else if (token == "quit" || token == "q")
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

			analyzer.setInitialPosition(Board{}.setToStartingPosition());
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

			if (_printPositions)
				printBoard(board);
		}
		else if (token == "go")
		{
			const Move bestMove = analyzer.findBestMove();
			const auto bestMoveStr = indexToSquareNotation(bestMove.from()) + indexToSquareNotation(bestMove.to());
			reply("bestmove ", bestMoveStr);
		}
		else if (token == "setoption")
		{
			std::string name, value;
			is >> std::skipws >> name;
			is >> std::skipws >> value;
		}
		else if (token == "d")
			printBoard(analyzer.board());
		else if (token == "ds") // "debug simple"
			printBoard(analyzer.board(), false);
		else if (token == "square" || token == "s")
		{
			std::string square;
			is >> std::skipws >> square;
			if (std::all_of(square.begin(), square.end(), ::isdigit))
				reply(indexToSquareNotation((uint8_t)std::stoi(square)));
			else
				reply((int)parseSquare(square));
		}
		else if (token == "response:")
			continue;
		else if (token == "printpositions")
		{
			is >> std::skipws >> token;
			if (token == "on")
				_printPositions = true;
			else
				_printPositions = false;
		}
		else if (token == "perft" || token == "perftd" /* perft debug */)
		{
			size_t depth = 3;
			is >> std::skipws >> depth;

			static const PerftPrintFunc printFunc = [](std::string_view move, uint64_t nodeCount) {
				reply(move, ": ", nodeCount);
			};

			const bool debugPrint = token == "perftd";

			for (size_t i = 1; i <= depth; ++i)
			{
				Board board = analyzer.board();

				CTimeElapsed timer(true);
				Perft results;
				perft(board, i, results, debugPrint ? printFunc : PerftPrintFunc{});
				const auto elapsed = timer.elapsed();

				reply(i, " - nodes: ", results.nodes
					   , ", captures: ", results.captures
					   , ", castles: ", results.castling
					   , ", en passant: ", results.enPassant
					   , ", time: ", elapsed, " ms"
				);
			}
		}
	}

	std::cout << std::endl;
}
