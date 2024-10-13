#include "logger.h"

#include <io.h>
#include <fcntl.h>

#include <filesystem>
#include <optional>

#ifdef _WIN32
#define fsync _commit
#endif

class LogFile {

public:
	LogFile() noexcept {
		auto p = std::filesystem::current_path();
		p.append("log.txt");
		fd = open(p.string().c_str(), _O_CREAT | O_TRUNC | _O_WRONLY | _O_BINARY, _S_IREAD | _S_IWRITE);
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