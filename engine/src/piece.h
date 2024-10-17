#pragma once

#include "piecetype.h"

#include <assert.h>

struct Piece
{
	// PieceType takes 3 bits, color is the 4th bit

	inline constexpr Piece() noexcept = default;

	inline constexpr Piece(PieceType piece, Color color) noexcept :
		p(piece | colorMask(color))
	{}

	[[nodiscard]] inline constexpr Color color() const noexcept {
		return static_cast<Color>(p >> 3);
	}

	[[nodiscard]] inline constexpr PieceType type() const noexcept {
		return static_cast<PieceType>(p & 0b111u);
	}

	[[nodiscard]] inline constexpr uint8_t id() const noexcept {
		return p;
	}

	[[nodiscard]] inline constexpr bool operator==(const Piece& other) const noexcept {
		return p == other.p;
	}

	[[nodiscard]] inline constexpr char notation() const noexcept {

		char letter = 0;
		switch (type()) {
			case Pawn: letter = 'P'; break;
			case Knight: letter = 'N'; break;
			case Bishop: letter = 'B'; break;
			case Rook: letter = 'R'; break;
			case Queen: letter = 'Q'; break;
			case King: letter = 'K'; break;
		}

		const char caseMask = color() == Color::White ? 0 : 0x20;
		return letter | caseMask;
	}

private:
	[[nodiscard]] inline static constexpr uint8_t colorMask(Color c) noexcept
	{
		return static_cast<uint8_t>(c << 3);
	}

private:
	uint8_t p = EmptySquare;
};

[[nodiscard]] inline constexpr Piece pieceFromLetter(char letter)
{
	const Color color = (letter & 0x20) == 0 /* upper case */ ? Color::White : Color::Black;

	letter |= 0x20; // Convert to lowercase
	switch (letter)
	{
	case 'p': return { PieceType::Pawn, color };
	case 'n': return { PieceType::Knight, color };
	case 'b': return { PieceType::Bishop, color };
	case 'r': return { PieceType::Rook, color };
	case 'q': return { PieceType::Queen, color };
	case 'k': return { PieceType::King, color };
	default:
		assert(false);
		return { PieceType::EmptySquare, color };
	}
};