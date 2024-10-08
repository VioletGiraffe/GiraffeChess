#pragma once

#include "piecetype.h"

struct Move {
	inline constexpr Move(uint8_t from_, uint8_t to_, bool capture = false, PieceType promotion_ = EmptySquare) noexcept :
		_from{from_}, _to{to_}, _isCapture{capture}, _promotion{promotion_}
	{}

	// Getters
	[[nodiscard]] constexpr uint8_t from() const noexcept { return _from; }
	[[nodiscard]] constexpr uint8_t to() const noexcept { return _to; }
	[[nodiscard]] constexpr PieceType promotion() const noexcept { return _promotion; }

	[[nodiscard]] constexpr bool isCapture() const noexcept { return _isCapture; }

	[[nodiscard]] constexpr bool isNull() const noexcept { return _from == 0 && _to == 0; }

private:
	uint16_t _from       : 6 = 0;
	uint16_t _to         : 6 = 0;
	PieceType _promotion : 3 = EmptySquare;  // If it's a pawn promotion move, specify the promoted piece type
	bool _isCapture      : 1 = false;
};

#ifndef _WIN32
static_assert(sizeof(Move) == 2);
#endif
