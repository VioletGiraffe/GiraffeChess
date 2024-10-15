#pragma once
#include <stdint.h>

class Board;

struct Perft {
	uint64_t nodes = 0;
	uint64_t enPassant = 0;
	uint64_t castling = 0;
};

void perft(Board& board, size_t depth, Perft& results) noexcept;
