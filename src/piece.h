#pragma once

#include "piecetype.h"

struct Piece
{
	// PieceType takes 3 bits, color is the 4th bit

	constexpr Piece() noexcept = default;

	constexpr Piece(PieceType piece, Color color) :
		p(piece | colorMask(color))
	{}

	[[nodiscard]] Color color() const {
		return static_cast<Color>(p >> 3);
	}

	[[nodiscard]] constexpr PieceType type() const {
		return static_cast<PieceType>(p & 0b111u);
	}

private:
	static constexpr uint8_t colorMask(Color c)
	{
		return static_cast<uint8_t>(c << 3);
	}

private:
	uint8_t p = EmptySquare;
};