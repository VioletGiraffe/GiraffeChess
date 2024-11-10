#include "analyzer.h"
#include "eval.h"
#include "threading/thread_helpers.h"

#include <assert/advanced_assert.h>

#include <iostream>
#include <ranges>

struct Node {
	inline constexpr Node() noexcept = default;

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

	float score = 0.0f;

	uint8_t level = 0;
	uint8_t moveIndex = 0;

	uint8_t flags = EvalFlags::None;
};

static void generateMoveTree(const Board& board, Node* parent, uint8_t depthLimit) noexcept
{
	if (isDrawPosition(board)) [[unlikely]]
	{
		parent->flags = EvalFlags::Draw;
		parent->score = 0.0f;
		return;
	}

	MoveList moves;
	board.generateMoves(board.sideToMove(), moves);

	// The moves are pseudo-legal! Count will never be 0 here

	const uint8_t depth = parent->level + 1;
	const bool leaf = depth >= depthLimit;

	uint8_t i = 0;
	for (; i < moves.count(); ++i)
	{
		// TODO: rewind the move instead of full copying
		Board nextBoard = board;
		if (!nextBoard.applyMove(moves[i]))
			continue;

		auto& newNode = parent->children.emplace_back(leaf ? eval(nextBoard) : 0.0f, depth, i, EvalFlags::None);
		if (!leaf)
			generateMoveTree(nextBoard, &newNode, depthLimit);
	}

	if (i == 0) [[unlikely]]
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
	}
}

static float calcMinMaxScore(Node* node, Color sideToMove) noexcept
{
	if (node->children.empty()) // Terminal node
		return node->score;

	const bool max = sideToMove == White;
	float result = max ? -1e30f : +1e30f;
	for (auto& child : node->children)
	{
		if (max)
			result = std::max(result, calcMinMaxScore(&child, oppositeSide(sideToMove)));
		else
			result = std::min(result, calcMinMaxScore(&child, oppositeSide(sideToMove)));
	}

	node->score = result;
	return result;
}

static void printTree(const Node& node, std::ostream& os, size_t level = 0) noexcept
{
	os << std::string(level * 2, ' ') << "move: " << (int)node.moveIndex;
	if (node.isMate())
		os << " mate";
	else if (node.isStalemate())
		os << " stalemate";
	else
		os << ' ' << node.score;

	os << '\n';

	for (const auto& child : node.children)
	{
		printTree(child, os, level + 1);
	}
}

Analyzer::Analyzer() noexcept
{
	_board.setToStartingPosition();
}

Analyzer::~Analyzer() noexcept
{
	stop();
}

void Analyzer::start() noexcept
{
	assert_r(!_thread.isRunning());
	_thread.start(&Analyzer::thread, this);
}

void Analyzer::stop() noexcept
{
	_thread.stop(true);
}

void Analyzer::startNewGame() noexcept
{
	assert_r(!_thread.isRunning()); // Analyzer must be stopped before starting a new game()
	_previousPositionHashes.clear();
}

void Analyzer::setInitialPosition(const Board& initialPosition) noexcept
{
	assert_r(!_thread.isRunning());
	_board = initialPosition;
}

Move Analyzer::findBestMove() noexcept
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

	static constexpr size_t depth = 4;

	Node tree;
	Node* currentNode = &tree;

	generateMoveTree(_board, currentNode, depth);
	calcMinMaxScore(currentNode, _board.sideToMove());

	uint8_t bestMoveIndex = 0;
	
	if (_board.sideToMove() == White)
	{
		const auto best = std::ranges::max_element(tree.children, std::less{}, &Node::score);
		bestMoveIndex = best->moveIndex;
	}
	else
	{
		const auto best = std::ranges::min_element(tree.children, std::less{}, &Node::score);
		bestMoveIndex = best->moveIndex;
	}

	MoveList moves;
	_board.generateMoves(_board.sideToMove(), moves);

	_bestMove = moves[bestMoveIndex];
}
