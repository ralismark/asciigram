#pragma once

#include "../canvas.hpp"
#include "../drawable.hpp"

#include <memory>
#include <utility>

struct BoxStyle
{
	// horizontal and vertical sides
	char tside = 0, bside = 0, rside = 0, lside = 0;
	// corners: top left, top right, bottom left, bottom right
	char tl_corner = 0, tr_corner = 0, bl_corner = 0, br_corner = 0;

	// what to fill the box (with spaces)
	char fill = 0;
};

struct Box
	: public Drawable
{
	// positions of opposite corners
	// DO NOT assume these are any specific corners, except after calling normalise()
	int x1, y1;
	int x2, y2;

	std::shared_ptr<BoxStyle> style;
public:
	Box(std::shared_ptr<BoxStyle> style)
		: x1(0), y1(0), x2(0), y2(0), style(style)
	{
	}

	Box(int x, int y, std::shared_ptr<BoxStyle> style)
		: x1(x), y1(y), x2(x), y2(y), style(style)
	{
	}

	Box(int x1, int y1, int x2, int y2, std::shared_ptr<BoxStyle> style)
		: x1(x1), y1(y1), x2(x2), y2(y2), style(style)
	{
	}

	int width() const
	{
		int raw_width = x1 - x2;
		return raw_width > 0 ? raw_width : -raw_width;
	}

	int height() const
	{
		int raw_height = y1 - y2;
		return raw_height > 0 ? raw_height : -raw_height;
	}

	// ensures that (x1, y1) are less than (x2, y2)
	void normalise()
	{
		if(x1 > x2) {
			std::swap(x1, x2);
		}
		if(y1 > y2) {
			std::swap(y1, y2);
		}
	}

	virtual void draw(Canvas& canvas) const override
	{
		auto norm = *this;
		norm.normalise();

		canvas.fill(style->fill, norm.x1, norm.y1, norm.x2, norm.y2);

		canvas.lineh(style->tside, norm.x1, norm.y1, norm.x2);
		canvas.lineh(style->bside, norm.x1, norm.y2, norm.x2);
		canvas.linev(style->lside, norm.x1, norm.y1, norm.y2);
		canvas.linev(style->rside, norm.x2, norm.y1, norm.y2);

		canvas.set(style->tl_corner, norm.x1, norm.y1);
		canvas.set(style->tr_corner, norm.x2, norm.y1);
		canvas.set(style->bl_corner, norm.x1, norm.y2);
		canvas.set(style->br_corner, norm.x2, norm.y2);
	}

	virtual void shift(int x, int y) override
	{
		x1 += x;
		x2 += x;
		y1 += y;
		y2 += y;
	}
};
