#include "debug.h"
#include "board.h"

#include <functional>
#include <iostream>

// ANSI escape codes for colors
static constexpr std::string_view RESET = "\033[0m";
static constexpr std::string_view LIGHT_SQUARE = "\033[48;5;231m";  // Light background
static constexpr std::string_view DARK_SQUARE = "\033[48;5;107m";   // Dark background

static constexpr std::string_view DoubleSizeTopHalf = "\033#3";
static constexpr std::string_view DoubleSizeBottomHalf = "\033#4";

inline constexpr std::string_view pieceToUnicode(const Piece& piece)
{
	switch (piece.id())
	{
	case 1:  return "♙";  // White pawn
	case 2:  return "♘";  // White knight
	case 3:  return "♗";  // White bishop
	case 4:  return "♖";  // White rook
	case 5:  return "♕";  // White queen
	case 6:  return "♔";  // White king
	case 9:  return "♟";  // Black pawn
	case 10: return "♞";  // Black knight
	case 11: return "♝";  // Black bishop
	case 12: return "♜";  // Black rook
	case 13: return "♛";  // Black queen
	case 14: return "♚";  // Black king
	default: return " ";   // Empty square
	}
}

static void printLnDoubleSize(std::function<void(std::string_view doubleSizeMan)>&& printer)
{
	printer(DoubleSizeTopHalf);
	printer(DoubleSizeBottomHalf);
}

void printBoard(const Board& board)
{
	auto& os = std::cout;

	printLnDoubleSize([&](std::string_view doubleMan) {
		// File labels
		os << doubleMan << "╭┈a┈b┈c┈d┈e┈f┈g┈h┈┈╮" << RESET << '\n';
	});

	for (int rank = 7; rank >= 0; --rank)
	{
		printLnDoubleSize([&](std::string_view doubleMan) {

			os << doubleMan << (rank + 1) << ' ' << RESET;  // Print rank number on the right side

			for (int file = 0; file < 8; ++file)
			{
				auto piece = board.pieceAt(rank * 8 + file);  // Access the piece at the board index
				bool isDarkSquare = (rank + file) % 2;   // Alternate squares based on file and rank

				// Set square background and piece
				os << doubleMan << (isDarkSquare ? DARK_SQUARE : LIGHT_SQUARE) << pieceToUnicode(piece) << ' ' << RESET;
			}

			os << doubleMan << " " << (rank + 1) << RESET << '\n';  // Print rank number on the right side
		});
	}

	printLnDoubleSize([&](std::string_view doubleMan) {
		// File labels
		os << doubleMan << "╰┈a┈b┈c┈d┈e┈f┈g┈h┈┈╯" << RESET << '\n';
	});
	
	os << std::endl;
}
