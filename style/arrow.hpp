#pragma once

#include "../style.hpp"

struct ArrowStyle
	: public Style
{
	// arrows
	char up = '^', down = 'v', right = '>', left = '<';
	// corners: top left, top right, bottom left, bottom right
	char tl = '.', tr = '.', bl = '\'', br = '\'';

	char vertical = '|', horizontal = '-';

	// what to put on markers
	char marker = 0;

	std::vector<std::pair<char*, point>> get_display_points()
	{
		return {
			{ &tl,         point(1, 1) },
			{ &up,         point(2, 1) },
			{ &tr,         point(3, 1) },
			{ &left,       point(1, 2) },
			{ &marker,     point(2, 2) },
			{ &right,      point(3, 2) },
			{ &bl,         point(1, 3) },
			{ &down,       point(2, 3) },
			{ &br,         point(3, 3) },
			{ &vertical,   point(5, 1) },
			{ &horizontal, point(5, 3) },
		};
	}
};
