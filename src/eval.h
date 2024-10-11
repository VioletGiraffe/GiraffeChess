#pragma once
#include <stdint.h>

class Board;
class Move;

enum EvalFlags : uint8_t {
	None = 0,
	Mate = 1,
	Stalemate = 2,
	Draw = 4
};

float eval(const Board& board) noexcept;
bool isDrawPosition(const Board& board) noexcept;
//EvalFlags specialConditions(const Board& board) noexcept;

Move findBestMove(const Board& board);
