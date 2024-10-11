#include "analyzer.h"
#include "eval.h"
#include "threading/thread_helpers.h"

#include <assert/advanced_assert.h>

#include <unordered_map>

struct Node {
	inline constexpr Node(float score_, uint8_t level_, uint8_t moveIndex_, uint8_t flags_) noexcept :
		score(score_), level(level_), moveIndex(moveIndex_), flags(flags_)
	{}

	[[nodiscard]] inline constexpr bool isMate() const noexcept {
		return flags & EvalFlags::Mate;
	}

	[[nodiscard]] inline constexpr bool isStalemate() const noexcept {
		return flags & EvalFlags::Stalemate;
	}

	std::vector<Node> children;

	float score;

	uint8_t level;
	uint8_t moveIndex;

	uint8_t flags;
};

static void generateMoveTree(const Board& board, Node* parent, uint8_t depthLimit)
{
	if (isDrawPosition(board)) [[unlikely]]
	{
		parent->flags = EvalFlags::Draw;
		parent->score = 0.0f;
		return;
	}

	MoveList moves;
	board.generateMoves(board.sideToMove(), moves);

	if (moves.count() == 0) [[unlikely]]
	{
		if (board.isInCheck(board.sideToMove()))
		{
			parent->flags |= EvalFlags::Mate;
			parent->score = board.sideToMove() == Color::White ? -1e5f : 1e5f;
		}
		else
		{
			parent->flags |= EvalFlags::Stalemate;
			parent->score = 0.0f;
		}

		return;
	}

	const uint8_t depth = parent->level + 1;

	for (uint8_t i = 0; i < moves.count(); ++i)
	{
		// TODO: rewind the move instead of full copying
		Board nextBoard = board;
		if (!nextBoard.applyMove(moves[i]))
			continue;

		auto& newNode = parent->children.emplace_back(eval(board), depth, i, EvalFlags::None);

		generateMoveTree(nextBoard, &newNode, depthLimit);
	}
}

//template <bool max>
//static float findMinMaxScore(Node* node, Color side)
//{
//	float result = max ? 1e30f : -1e30f;
//
//	if (node->children.empty()) [[unlikely]] // Terminal node
//	{
//		result = side == Color::White ? node->score : -node->score;
//	}
//	else
//	{
//		for (auto& child : node->children)
//		{
//			const float score = findMinMaxScore(&child, side);
//			result = max ? std::max(result, score) : std::min(result, score);
//		}
//	}
//
//	return result;
//}

Analyzer::Analyzer()
{
	_board.setToStartingPosition();
}

Analyzer::~Analyzer()
{
	stop();
}

void Analyzer::start()
{
	assert_r(!_thread.isRunning());
	_thread.start(&Analyzer::thread, this);
}

void Analyzer::stop()
{
	_thread.stop(true);
}

void Analyzer::setInitialPosition(const Board& initialPosition)
{
	assert_r(!_thread.isRunning());
	_board = initialPosition;
}

Move Analyzer::findBestMove()
{
	start();
	// Wait for the thread to finish
	_thread.join();

	return _bestMove;
}

const Board& Analyzer::board() const noexcept
{
	return _board;
}

void Analyzer::thread() noexcept
{
	setThreadName("Analyzer thread");

	::findBestMove(_board);
	return;

	constexpr size_t depth = 3;

	Node tree(eval(_board), 0, 0, EvalFlags::None);
	tree.level = 0;

	Node* currentNode = &tree;

	while (!_thread.terminationRequested())
	{
	}
}
