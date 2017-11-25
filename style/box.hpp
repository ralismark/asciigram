#pragma once

#include "../style.hpp"

struct BoxStyle
	: public Style
{
	// horizontal and vertical sides
	char tside = '-', bside = '-', rside = '|', lside = '|';
	// corners: top left, top right, bottom left, bottom right
	char tl_corner = '.', tr_corner = '.', bl_corner = '\'', br_corner = '\'';

	// what to fill the box (with spaces)
	char fill = 0;

	std::vector<std::pair<char*, point>> get_display_points()
	{
		return {
			{ &tl_corner, point(1, 1) },
			{ &tside,     point(2, 1) },
			{ &tr_corner, point(3, 1) },
			{ &lside,     point(1, 2) },
			{ &fill,      point(2, 2) },
			{ &rside,     point(3, 2) },
			{ &bl_corner, point(1, 3) },
			{ &bside,     point(2, 3) },
			{ &br_corner, point(3, 3) },
		};
	}
};
