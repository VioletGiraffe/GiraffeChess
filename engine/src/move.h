#pragma once

#include "piecetype.h"
#include "notation.h"

#include <assert.h>

class Move {
public:
	inline constexpr Move() noexcept = default;

	inline constexpr Move(uint8_t from_, uint8_t to_, bool capture = false, PieceType promotion_ = EmptySquare) noexcept :
		_from{from_}, _to{to_}, _promotion{promotion_}, _isCapture{capture}
	{}

	// Getters
	[[nodiscard]] constexpr uint8_t from() const noexcept { return _from; }
	[[nodiscard]] constexpr uint8_t to() const noexcept { return _to; }
	[[nodiscard]] constexpr PieceType promotion() const noexcept { return static_cast<PieceType>(_promotion); }

	[[nodiscard]] constexpr bool isCapture() const noexcept { return _isCapture; }

	[[nodiscard]] constexpr bool isNull() const noexcept { return _from == 0 && _to == 0; }

	[[nodiscard]] std::string notation() const noexcept {
		static constexpr auto pieceTypeNotation = [](PieceType type) noexcept -> char {
			switch (type)
			{
			case PieceType::Bishop: return 'b';
			case PieceType::Knight: return 'n';
			case PieceType::Rook: return 'r';
			case PieceType::Queen: return 'q';
			default:
				assert(false);
				return 0;
			}
		};

		std::string str = indexToSquareNotation(_from) + indexToSquareNotation(_to);
		if (_promotion != EmptySquare)
			str += pieceTypeNotation(promotion());

		return str;
	}

private:
	// Has to be uint16_t to be packed into 2 bytes
	uint16_t _from       : 6;
	uint16_t _to         : 6;
	uint16_t _promotion : 3;  // If it's a pawn promotion move, specify the promoted piece type
	uint16_t _isCapture      : 1;
};

static_assert(sizeof(Move) == 2);
