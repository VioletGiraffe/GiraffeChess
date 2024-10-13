#include "logger.h"

#include <fcntl.h>

#include <optional>

#ifdef _WIN32
#include <io.h>
#define fsync _commit
#endif

class LogFile {

public:
	LogFile() noexcept {
		fd = open("log.txt", O_CREAT | O_TRUNC | O_WRONLY | O_BINARY, S_IREAD | S_IWRITE);
	}

	~LogFile() noexcept {
		if (fd != -1)
			close(fd);
	}

	void write(std::string_view message) noexcept {
		if (fd == -1)
			return;

		[[maybe_unused]] int ret = ::write(fd, message.data(), (unsigned int)message.size());
		//fsync(fd);
	}

private:
	int fd = -1;
};

static std::optional<LogFile> logFile;

void logToFile(std::string_view text)
{
	if (!logFile) [[unlikely]]
		logFile.emplace();

	logFile->write(text);
}