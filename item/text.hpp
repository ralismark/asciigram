#pragma once

#include "../canvas.hpp"
#include "../drawable.hpp"

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

	virtual void draw(Canvas& canvas) const override
	{
		canvas.direct(string, x, y);
	}

	virtual void shift(int ox, int oy) override
	{
		x += ox;
		y += oy;
	}
};
