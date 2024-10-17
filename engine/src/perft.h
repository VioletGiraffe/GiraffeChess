#pragma once
#include <stdint.h>
#include <functional>
#include <string_view>

class Board;

struct PerftResults {
	uint64_t nodes = 0;
	uint64_t enPassant = 0;
	uint64_t castling = 0;
	uint64_t captures = 0;
};

using PerftPrintFunc = std::function<void (std::string_view move, uint64_t nodes)>;

void perft(Board& board, size_t depth, PerftResults& results, const PerftPrintFunc& printFunc = {}) noexcept;
