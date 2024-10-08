#include "analyzer.h"
#include "threading/thread_helpers.h"

#include <assert/advanced_assert.h>

Analyzer::Analyzer()
{
}

Analyzer::~Analyzer()
{
	stop();
}

void Analyzer::start()
{
	assert_r(!_uciThread.isRunning());
	_uciThread.start(&Analyzer::thread, this);
}

void Analyzer::stop()
{
	_uciThread.stop(true);
}

void Analyzer::setInitialPosition(const Board& initialPosition)
{
	assert_r(!_uciThread.isRunning());
	_board = initialPosition;
}

void Analyzer::thread() noexcept
{
	setThreadName("Analyzer thread");

	while (!_uciThread.terminationRequested())
	{

	}
}
