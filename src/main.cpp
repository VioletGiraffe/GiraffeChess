#include "uci.h"

#include <Windows.h>

int main(int, char*[])
{
#ifdef _DEBUG
	::MessageBoxA(nullptr, "Attach debugger now", "Attach debugger now", MB_OK | MB_ICONINFORMATION);
#endif

	UciServer uciServer;
	uciServer.run();

	return 0;
}