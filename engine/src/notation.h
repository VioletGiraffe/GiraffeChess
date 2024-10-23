#pragma once
#include "piece.h"

#include <assert.h>
#include <sstream>
#include <string>
#include <string_view>

class Board;

[[nodiscard]] inline constexpr uint8_t parseSquare(std::string_view square)
{
	const uint8_t file = square[0] - (uint8_t)'a'; // File 'a' = 0, 'b' = 1, etc.
	const uint8_t rank = square[1] - (uint8_t)'1'; // Rank '1' = 0, '2' = 1, etc.
	return rank * 8 + file; // Convert to a 0-63 index
}

[[nodiscard]] inline std::string indexToSquareNotation(uint8_t index)
{
	const char file = 'a' + (index % 8);
	const char rank = '1' + (index / 8);

	std::string square = { file, rank };
	return square;
}

[[nodiscard]] std::string generateFEN(const Board& board);
void parseFEN(std::istringstream& iss, Board& board);
