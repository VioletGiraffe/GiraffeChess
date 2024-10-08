#pragma once
#include "analyzer.h"

#include "threading/simplethread.h"

class UciServer
{
public:
	UciServer();

private:
	void uci_loop();

	void uciThread() noexcept;

private:
	SimpleThread _uciThread;
};
