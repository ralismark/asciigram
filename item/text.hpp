#pragma once

/**
 * \file
 * This file defines Text, a Drawable which represents a block of raw text.
 */

#include "../canvas.hpp"
#include "../drawable.hpp"

/**
 * Stores a block of text.
 *
 * This type is able to store a string, which may contain line endings. When
 * rendering, this is handled correctly, aligning them to the left.
 */
struct Text
	: public Drawable
{
	std::string string;
	int x, y;
public:
	Text(int x, int y)
		: string(), x(x), y(y)
	{
	}

	/**
	 * Create a copy.
	 */
	virtual std::unique_ptr<Drawable> clone() const
	{
		return std::make_unique<Text>(*this);
	}

	/**
	 * Draw the text, correctly handling newlines.
	 */
	virtual void draw(Canvas& canvas) const override
	{
		int line_y = y;
		size_t start = 0;

		do {
			size_t end = string.find('\n', start);
			canvas.direct(string.substr(start, end - start), x, line_y);
			start = end + 1;

			++line_y;
		} while(start != std::string::npos + 1);
	}

	/**
	 * Offset the element by a (\p ox, \p oy).
	 */
	virtual void shift(int ox, int oy) override
	{
		x += ox;
		y += oy;
	}
};
