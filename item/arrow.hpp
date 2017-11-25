#pragma once

#include "../base.hpp"
#include "../canvas.hpp"
#include "../drawable.hpp"

#include "../style/arrow.hpp"

#include <algorithm>

// TODO: Add styling

struct Arrow
	: public Drawable
{
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
