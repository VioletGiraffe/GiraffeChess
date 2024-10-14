#pragma once
#include <stdint.h>

class Board;

uint64_t perft(Board& board, size_t depth) noexcept;
