#pragma once
#include "analyzer.h"

class UciServer
{
public:
	UciServer();
	void run();

private:
	void uci_loop();

private:
	bool _printPositions = false;
};
