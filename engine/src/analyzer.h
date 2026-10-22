#pragma once

#include "board.h"
#include "threading/cinterruptablethread.h"

#include <functional>
#include <stdint.h>
#include <vector>

class Analyzer
{
public:
	// Runs on the search thread
	using SearchFinishedCallback = std::function<void(Move bestMove, uint64_t elapsedMs)>;

	Analyzer() noexcept;
	~Analyzer() noexcept;

	// startSearch, startNewGame and setInitialPosition stop the search in flight first, if any
	void startSearch(SearchFinishedCallback onFinished) noexcept;
	// Returns at once; the search still reports its result through the callback
	void requestStop() noexcept;
	// Returns after the callback of the search in flight, if any, has returned
	void stopSearch() noexcept;

	void startNewGame() noexcept;
	void setInitialPosition(const Board& initialPosition) noexcept;
	[[nodiscard]] const Board& board() const noexcept;

private:
	[[nodiscard]] Move findBestMove() const noexcept;

private:
	std::vector<uint64_t> _previousPositionHashes; // Needed to detect repetitions, TODO: flat_set? Heap?

	CInterruptableThread _thread{ "Analyzer thread" };
	Board _board;
};