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

[[nodiscard]] float eval(const Board& board) noexcept;
[[nodiscard]] bool isDrawPosition(const Board& board) noexcept;
//EvalFlags specialConditions(const Board& board) noexcept;

[[nodiscard]] Move findBestMove(const Board& board);
