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
	Move findBestMove();

private:
	void start();
	void thread() noexcept;

private:
	SimpleThread _thread;
	Board _board;
	Move _bestMove;
};