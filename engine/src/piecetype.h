#pragma once

#include <stdint.h>

enum PieceType : uint8_t {
	EmptySquare,
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King
};

enum Color : uint8_t {
	White = 0,
	Black = 1
};

inline constexpr Color oppositeSide(Color side) noexcept
{
	return side == Color::White ? Color::Black : Color::White;
}