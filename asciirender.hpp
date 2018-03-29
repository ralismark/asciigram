#pragma once

#include "canvas.hpp"

#include <algorithm>

/**
 * Render a rectangular region as plain text.
 *
 * This Canvas takes an inclusive rectangular region, and draws elements into
 * that region.  This is useful for when copying to clipboard.
 */
struct AsciiRenderer
	: public Canvas
{
	point min, max; // half-open range
	std::vector<std::string> lines;
public:
	// min{x,y}/max{x,y} here are inclusive, so we need to change the range a bit
	AsciiRenderer(int x1, int y1, int x2, int y2)
		: min(std::min(x1, x2), std::min(y1, y2))
		, max(std::max(x1, x2) + 1, std::max(y1, y2) + 1)
		, lines(max.y - min.y)
	{
	}

	/**
	 * Ensure that a given line \p lineno is at least \p length long.
	 */
	void extend_line(int lineno, size_t length)
	{
		auto& line = lines[lineno];
		if(line.size() <= length) {
			line.resize(length + 1, ' ');
		}
	}

	// TODO: Make more efficient if needed
	virtual void impl_set(char fill, int x, int y) override
	{
		if(x < min.x || max.x <= x) {
			return;
		}

		if(y < min.y || max.y <= y) {
			return;
		}

		point offset = { x - min.x, y - min.y };

		this->extend_line(offset.y, offset.x);
		lines[offset.y][offset.x] = fill;
	}

	/**
	 * Get the rendered text, join by new lines into a single string.
	 */
	std::string joined() const
	{
		std::string out;
		for(auto& line : lines) {
			out += line;
			out += '\n';
		}
		return out;
	}
};
