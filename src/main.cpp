#include "uci.h"
#include "debugger/debugger_is_attached.h"

#ifdef _WIN32
#include <Windows.h>
#endif

int main(int, char*[])
{
#if defined _DEBUG && defined _WIN32
	if (!debuggerIsAttached())
		::MessageBoxA(nullptr, "Attach debugger now", "Attach debugger now", MB_OK | MB_ICONINFORMATION);

	SetConsoleOutputCP(CP_UTF8);
#endif

	UciServer uciServer;
	uciServer.run();

	return 0;
}