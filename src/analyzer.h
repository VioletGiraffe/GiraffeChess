#pragma once

#include "board.h"
#include "threading/simplethread.h"

class Analyzer
{
public:
	Analyzer();
	~Analyzer();

	void start();
	void stop();

	void setInitialPosition(const Board& initialPosition);

private:
	void thread() noexcept;

private:
	SimpleThread _uciThread;
	Board _board;
};