#define NO_TEST_MAIN
#include "3rdparty/catch2/test_main.hpp"

#include "notation.h"
#include "perft.h"
#include "board.h"
#include "system/cpuinfo.hpp"
#include "timing/ctimeelapsed.h"

#include <charconv>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <vector>

static size_t s_maxDepth = std::numeric_limits<size_t>::max();

namespace {

struct TestPosition {
	struct Result {
		size_t depth = 0;
		uint64_t nodes = 0;
	};

	std::string fen;
	std::vector<Result> nodeCountForDepth;
};

}

static std::vector<TestPosition> parsePositions(const char* path)
{
	std::ifstream file(path);
	if (!file.is_open())
		FAIL("Could not open " << path);

	std::vector<TestPosition> positions;

	std::string line;
	while (std::getline(file, line))
	{
		std::string token;
		std::istringstream ss{line};
		if (line.empty() || line.front() == '#')
			continue;

		while (std::getline(ss, token, ';'))
		{
			std::istringstream localstream{ token };
			if (token.starts_with('D'))
			{
				std::string depthString, nodeCountString;
				localstream >> std::skipws >> depthString >> nodeCountString;

				uint32_t depth = 0;
				std::from_chars(depthString.data() + 1, depthString.data() + depthString.size(), depth);

				uint64_t nodeCount = 0;
				std::from_chars(nodeCountString.data(), nodeCountString.data() + nodeCountString.size(), nodeCount);

				positions.back().nodeCountForDepth.push_back({ depth, nodeCount });
			}
			else
			{
				// FEN
				positions.push_back({ token, {} });
			}
		}
	}

	return positions;
}

static uint64_t checkPosition(const TestPosition& position)
{
	Board board;
	std::istringstream iss{ position.fen };
	parseFEN(iss, board);

	uint64_t nodesVisited = 0;
	for (const auto& depth: position.nodeCountForDepth)
	{
		if (depth.depth > s_maxDepth)
			continue;

		INFO(position.fen << ", depth " << depth.depth);
		PerftResults results;
		perft(board, depth.depth, results);
		CHECK(results.nodes == depth.nodes);
		nodesVisited += results.nodes;
	}

	return nodesVisited;
}

TEST_CASE("perft", "[perft]")
{
	const auto positions = parsePositions(PERFT_EPD_PATH);

	CTimeElapsed timer(true);
	uint64_t nodesVisited = 0;
	for (size_t i = 0; i < positions.size(); ++i)
	{
		std::cout << "Checking " << positions[i].fen << " (" << i << " out of " << positions.size() << ")...\n";
		nodesVisited += checkPosition(positions[i]);
	}

	// CI greps this line into the job summary
	const double seconds = static_cast<double>(timer.msElapsed()) * 1e-3;
	std::cout << std::fixed << std::setprecision(1)
		<< "perf: " << nodesVisited << " nodes in " << seconds << " s, " << static_cast<double>(nodesVisited) * 1e-6 / seconds << " MNps on " << cpuModelName() << std::endl;
}

int main(int argc, char* argv[])
{
	Catch::Session session;
	session.cli(session.cli() | Catch::clara::Opt(s_maxDepth, "depth")["--max-depth"]("skip perft depths above this"));
	return runCatchSession(session, argc, argv);
}