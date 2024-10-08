#pragma once

inline constexpr int knightMoves[8][2] {
	{-2, -1},
	{-2, 1},
	{-1, -2},
	{-1, 2},
	{1, -2},
	{1, 2},
	{2, -1},
	{2, 1}
};

inline constexpr int bishopMoveVectors[4][2] {
	{-1, -1}, {-1, 1}, {1, -1}, {1, 1}
};

inline constexpr int rookMoveVectors[4][2] {
	{-1, 0}, {0, -1}, {1, 0}, {0, 1}
};

inline constexpr int pawnAttackVectors[2][2] {
	{1, 1}, {1, -1}
};