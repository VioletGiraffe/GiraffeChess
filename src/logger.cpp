#include "logger.h"

#include <io.h>
#include <fcntl.h>

#ifdef _WIN32
#define fsync _commit
#endif

class LogFile {

public:
	LogFile() noexcept {
		fd = open("log.txt", _O_CREAT | O_TRUNC | _O_WRONLY);
	}

	~LogFile() noexcept {
		if (fd != -1)
			close(fd);
	}

	void write(std::string_view message) noexcept {
		if (fd == -1)
			return;

		[[maybe_unused]] int ret = ::write(fd, message.data(), (unsigned int)message.size());
		fsync(fd);
	}

private:
	int fd = -1;
};

static LogFile logFile;

void logToFile(std::string_view text)
{
	logFile.write(text);
}