#pragma once

#include "board.h"
#include "threading/simplethread.h"

#include <vector>

class Analyzer
{
public:
	Analyzer() noexcept;
	~Analyzer() noexcept;

	void stop() noexcept;

	void startNewGame() noexcept;
	void setInitialPosition(const Board& initialPosition) noexcept;
	[[nodiscard]] Move findBestMove() noexcept;
	[[nodiscard]] const Board& board() const noexcept;

private:
	void start() noexcept;
	void thread() noexcept;

private:
	std::vector<uint64_t> _previousPositionHashes; // Needed to detect repetitions, TODO: flat_set? Heap?

	SimpleThread _thread;
	Board _board;
	Move _bestMove = {0, 0, false, EmptySquare};
};