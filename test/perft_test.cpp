#define CATCH_CONFIG_MAIN
#include "3rdparty/catch2/catch.hpp"

#include "notation.h"
#include "perft.h"
#include "notation.h"
#include "board.h"
#include "system/ctimeelapsed.h"

#include <fstream>
#include <iostream>
#include <vector>

struct TestPosition {
	struct Result {
		size_t depth = 0;
		uint64_t nodes = 0;
	};

	std::string fen;
	std::vector<Result> nodeCountForDepth;
};

static std::vector<TestPosition> parsePositions(std::string_view path)
{
	std::ifstream file(path.data());
	if (!file.is_open())
		FAIL("Could not open file");

	std::vector<TestPosition> positions;

	std::string line;
	while (std::getline(file, line))
	{
		std::string token;
		std::istringstream ss{line};
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

static void checkPosition(const TestPosition& position)
{
	Board board;
	std::istringstream iss{ position.fen };
	parseFEN(iss, board);

	for (const auto& depth: position.nodeCountForDepth)
	{
		std::cout << "depth " << depth.depth << std::endl;
		PerftResults results;
		perft(board, depth.depth, results);
		CHECK(results.nodes == depth.nodes);
	}
}

TEST_CASE("perft", "[perft]")
{
	const auto positions = parsePositions("../test/standard.epd");

	CTimeElapsed timer(true);
	for (size_t i = 0; i < positions.size(); ++i)
	{
		std::cout << "Checking " << positions[i].fen << " (" << i << " out of " << positions.size() << ")...\n";
		checkPosition(positions[i]);
	}

	std::cout << "Total time: " << timer.elapsed() * 1e-3f << " seconds" << std::endl;
}