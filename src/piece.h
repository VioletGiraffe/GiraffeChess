#pragma once

#include "piecetype.h"

struct Piece
{
	// PieceType takes 3 bits, color is the 4th bit

	inline constexpr Piece() noexcept = default;

	inline constexpr Piece(PieceType piece, Color color) noexcept :
		p(piece | colorMask(color))
	{}

	inline [[nodiscard]] Color color() const noexcept {
		return static_cast<Color>(p >> 3);
	}

	inline [[nodiscard]] constexpr PieceType type() const noexcept {
		return static_cast<PieceType>(p & 0b111u);
	}

	inline [[nodiscard]] constexpr uint8_t id() const noexcept {
		return p;
	}

private:
	inline [[nodiscard]] static constexpr uint8_t colorMask(Color c) noexcept
	{
		return static_cast<uint8_t>(c << 3);
	}

private:
	uint8_t p = EmptySquare;
};