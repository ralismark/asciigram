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

struct TextBlock
	: public Drawable
{
	std::vector<std::string> block;
	int x, y;
public:
	TextBlock(int x, int y)
		: block(), x(x), y(y)
	{
	}

	virtual void draw(Canvas& canvas) const override
	{
		int ypos = y;
		for(auto& row : block) {
			canvas.direct(row, x, ypos);
			++ypos;
		}
	}

	virtual void shift(int ox, int oy) override
	{
		x += ox;
		y += oy;
		mvprintw(3, 3, "Shift %d %d", ox, oy);
	}
};
