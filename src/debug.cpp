#include "debug.h"
#include "board.h"

#include <iostream>

// ANSI escape codes for colors
static constexpr std::string_view RESET = "\033[0m";
static constexpr std::string_view LIGHT_SQUARE = "\033[48;5;187m";  // Light background
static constexpr std::string_view DARK_SQUARE = "\033[48;5;101m";   // Dark background

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

void printBoard(const Board& board)
{
	auto& os = std::cout;

	os << "  a b c d e f g h\n";  // Print the file labels
	os << " +----------------+\n";

	for (int rank = 7; rank >= 0; --rank) // Loop over ranks (from 8 to 1)
	{
		os << (rank + 1) << '|';  // Print rank label

		for (int file = 0; file < 8; ++file)
		{
			const auto piece = board.pieceAt(static_cast<uint8_t>(rank * 8 + file));
			const bool isDarkSquare = (rank + file) % 2;   // Alternate squares based on file and rank

			// Set background color for the square
			os << (isDarkSquare ? DARK_SQUARE : LIGHT_SQUARE);
			os << pieceToUnicode(piece) << ' ';  // Print the piece followed by a space
			os << RESET;  // Reset formatting
		}

		os << '|' << (rank + 1) << '\n';  // End the rank with its label
	}

	os << " +----------------+\n";
	os << "  a b c d e f g h\n";  // Print the file labels again
	
	os << std::endl;
}
