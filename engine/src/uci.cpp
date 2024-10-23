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
		if (command.empty() || command[0] == '#')
			continue;

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
		{
			printBoard(analyzer.board());
			reply(generateFEN(analyzer.board()));
		}
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
				PerftResults results;
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
