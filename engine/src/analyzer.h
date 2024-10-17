#pragma once

#include "board.h"
#include "threading/simplethread.h"

class Analyzer
{
public:
	Analyzer();
	~Analyzer();

	void stop();

	void setInitialPosition(const Board& initialPosition);
	[[nodiscard]] Move findBestMove();
	[[nodiscard]] const Board& board() const noexcept;

private:
	void start();
	void thread() noexcept;

private:
	SimpleThread _thread;
	Board _board;
	Move _bestMove = {0, 0, false, EmptySquare};
};