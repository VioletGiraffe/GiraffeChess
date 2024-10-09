#pragma once
#include "analyzer.h"

#include "threading/simplethread.h"

class UciServer
{
public:
	UciServer();
	void run();

private:
	void uci_loop();
};
