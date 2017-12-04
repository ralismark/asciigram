#pragma once

/**
 * \file
 * This file defines the Arrow Drawable, which represents an arrow (with many
 * fancy features). Styling is in ../style/arrow.hpp.
 */

#include "../base.hpp"
#include "../canvas.hpp"
#include "../drawable.hpp"

#include "../style/arrow.hpp"

#include <algorithm>

/**
 * Stores a complex arrow.
 *
 * This stores an arrow as a sequence of points to pass through, as well as
 * orientation of each section. This representation allows complex arrow
 * layouts to be made, though not always in a simple manner.
 */
struct Arrow
	: public Drawable
{
	/// Orientation to go first for a segment
	enum Direction
	{
		Vertical,
		Horizontal,
	};
public:
	point start;
	std::vector<std::pair<point, Direction>> points; // points to pass through, with direction

	std::shared_ptr<ArrowStyle> style;
public:
	Arrow(int x, int y, std::shared_ptr<ArrowStyle> style)
		: start(x, y), points(), style(style)
	{
	}

	/**
	 * Create a copy of the Drawable.
	 */
	virtual std::unique_ptr<Drawable> clone() const
	{
		return std::make_unique<Arrow>(*this);
	}

	/**
	 * Flip the orientation of the last segment. This requires there to be
	 * at least one segment.
	 */
	void flip_last()
	{
		auto& dir = points.back().second;
		if(dir == Vertical) {
			dir = Horizontal;
		} else {
			dir = Vertical;
		}
	}

	/**
	 * Add a segment.
	 */
	void add_point(int x, int y, Direction dir = Vertical)
	{
		points.emplace_back(point{x, y}, dir);
	}

	/**
	 * Get notable points. This includes all markers, start/end points and
	 * corners.
	 */
	std::vector<point> get_all_markers() const
	{
		std::vector<point> markers{1, start};

		auto from = start;

		for(auto& segment : points) {
			auto& to = segment.first;

			// get corner
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

	/**
	 * Draw the arrow.
	 *
	 * The parts of the arrow are drawn using the style provided.
	 *
	 * Currently, corners and arrowheads are not implemented.
	 */
	virtual void draw(Canvas& canvas) const override
	{
		if(points.empty()) {
			return;
		}

		point from = start;

		for(auto& segment : points) {
			auto& to = segment.first;

			if(segment.second == Vertical) {
				canvas.linev(style->vertical, from.x, std::min(from.y, to.y), std::max(from.y, to.y));
				canvas.lineh(style->horizontal, std::min(from.x, to.x), to.y, std::max(from.x, to.x));
			} else {
				canvas.lineh(style->horizontal, std::min(from.x, to.x), from.y, std::max(from.x, to.x));
				canvas.linev(style->vertical, to.x, std::min(from.y, to.y), std::max(from.y, to.y));
			}

			from = to;
		}

		// TODO: Corners and arrows
		// for(auto& marker : this->get_all_markers()) {
		// }

		// draw markers
		for(auto& segment : points) {
			auto& mark = segment.first;
			canvas.set(style->marker, mark.x, mark.y);
		}

	}

	/**
	 * Move entire arrow by specific amount.
	 */
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
