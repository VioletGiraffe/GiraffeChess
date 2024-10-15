#include "uci.h"
#include "debugger/debugger_is_attached.h"

#ifdef _WIN32
#include <Windows.h>
#include <io.h>
#else
#include <unistd.h>
#endif

#include <fcntl.h>
#include <iostream>

#ifndef O_TEXT
#define O_TEXT 0
#endif

int main(int argc, char* argv[])
{
	int fd = -1;
	if (argc > 1)
	{
		fd = open(argv[1], O_RDONLY | O_TEXT);
		if (fd == -1)
		{
			std::cout << "Error opening file: " << argv[1] << std::endl;
			return 1;
		}

		const int stdin_fd = fileno(stdin);
		if (stdin_fd == -1)
		{
			std::cout << "Error getting stdin file descriptor" << std::endl;
			return 1;
		}

		if (dup2(fd, stdin_fd) == -1)
		{
			std::cout << "Error redirecting stdin" << std::endl;
			return 1;
		}
	}

#ifdef _WIN32

#ifdef _DEBUG
	if (!debuggerIsAttached())
		::MessageBoxA(nullptr, "Attach debugger now", "Attach debugger now", MB_OK | MB_ICONINFORMATION);
#endif

	SetConsoleOutputCP(CP_UTF8);
#endif

	UciServer uciServer;
	uciServer.run();

	if (fd != -1)
		close(fd);

	return 0;
}
