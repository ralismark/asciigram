#pragma once

#include "../base.hpp"
#include "../canvas.hpp"
#include "../drawable.hpp"

#include <algorithm>

// TODO: Add styling

// // TODO: maybe reuse BoxStyle if we're going with 8-directional arrows
// struct HeadStyle
// {
// 	// Symbol for each arrow direction
// 	char left = 0, down = 0, up = 0, right = 0;
// };

// // style for main part of arrow
// struct LineStyle
// {
// 	// Symbol for arrow directions
// 	char vertical = 0, horizontal = 0;
// 	// corners: top left, top right, bottom left, bottom right
// 	char tl_corner = 0, tr_corner = 0, bl_corner = 0, br_corner = 0;
// };

// struct ArrowStyle
// {
// 	// Style for first and last symbol
// 	HeadStyle head, tail;
// 	LineStyle line;
// };

// struct WhirlArrow
// 	: public Drawable
// {
// 	// Which way to go when confused
// 	enum Direction
// 	{
// 		Vertical,
// 		Horizontal,
// 	};
// public:
// 	std::vector<std::pair<int, int>> points;
// 	Direction direction;
// 	std::shared_ptr<ArrowStyle> style;
// public:
// 	Arrow(int x, int y, std::shared_ptr<ArrowStyle> style)
// 		: x1(x), y1(y), x2(x), y2(y), direction(Horizontal), style(style)
// 	{
// 	}
// };

struct Arrow
	: public Drawable
{
	struct Style
	{
		// arrows
		char up = 0, down = 0, right = 0, left = 0;
		// corners: top left, top right, bottom left, bottom right
		char tl = 0, tr = 0, bl = 0, br = 0;

		char vertical = 0, horizontal = 0;

		// what to fill the box (with spaces)
		char marker = 0;
	};

	// whether to go vertical or horizontal first
	enum Direction
	{
		Vertical,
		Horizontal,
	};
public:
	point start;
	std::vector<std::pair<point, Direction>> points; // points to pass through, with direction

	// std::shared_ptr<ArrowStyle> style;
public:
	Arrow(int x, int y/*, std::shared_ptr<ArrowStyle> style*/)
		: start(x, y), points()/*, style(style)*/
	{
	}

	void flip_last()
	{
		auto& dir = points.back().second;
		if(dir == Vertical) {
			dir = Horizontal;
		} else {
			dir = Vertical;
		}
	}

	void add_point(int x, int y, Direction dir = Vertical)
	{
		points.emplace_back(point{x, y}, dir);
	}

	// start, end, and corners
	std::vector<point> get_all_markers() const
	{
		std::vector<point> markers{1, start};

		auto from = start;

		for(auto& segment : points) {
			auto& to = segment.first;

			if(segment.second == Vertical) {
				markers.emplace_back(from.x, to.y);
			} else {
				markers.emplace_back(to.x, from.y);
			}
			markers.emplace_back(to);

			from = to;
		}
		return markers;
	}

	virtual void draw(Canvas& canvas) const override
	{
		if(points.empty()) {
			return;
		}

		point from = start;

		for(auto& segment : points) {
			auto& to = segment.first;

			if(segment.second == Vertical) {
				canvas.linev('|', from.x, std::min(from.y, to.y), std::max(from.y, to.y) + 1);
				canvas.lineh('-', std::min(from.x, to.x), to.y, std::max(from.x, to.x) + 1);
			} else {
				canvas.lineh('-', std::min(from.x, to.x), from.y, std::max(from.x, to.x) + 1);
				canvas.linev('|', to.x, std::min(from.y, to.y), std::max(from.y, to.y) + 1);
			}

			from = to;
		}

		// draw markers
		for(auto& marker : this->get_all_markers()) {
			canvas.set('o', marker.x, marker.y);
		}
	}

	virtual void shift(int x, int y) override
	{
		start.x += x;
		start.y += y;

		for(auto& point : points) {
			point.first.x += x;
			point.first.y += y;
		}
	}
};
