#pragma once

#include "piecetype.h"
#include "notation.h"

class Move {
public:
	inline constexpr Move() noexcept = default;

	inline constexpr Move(uint8_t from_, uint8_t to_, bool capture = false, PieceType promotion_ = EmptySquare) noexcept :
		_from{from_}, _to{to_}, _promotion{promotion_}, _isCapture{capture}
	{}

	// Getters
	[[nodiscard]] constexpr uint8_t from() const noexcept { return _from; }
	[[nodiscard]] constexpr uint8_t to() const noexcept { return _to; }
	[[nodiscard]] constexpr PieceType promotion() const noexcept { return _promotion; }

	[[nodiscard]] constexpr bool isCapture() const noexcept { return _isCapture; }

	[[nodiscard]] constexpr bool isNull() const noexcept { return _from == 0 && _to == 0; }

	[[nodiscard]] constexpr std::string notation() const noexcept {
		return indexToSquareNotation(_from) + indexToSquareNotation(_to);
	}

private:
	uint16_t _from       : 6;
	uint16_t _to         : 6;
	PieceType _promotion : 3;  // If it's a pawn promotion move, specify the promoted piece type
	bool _isCapture      : 1;
};

#ifndef _WIN32
static_assert(sizeof(Move) == 2);
#endif
