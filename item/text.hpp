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

	virtual std::unique_ptr<Drawable> clone() const
	{
		return std::make_unique<Text>(*this);
	}

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

	virtual void shift(int ox, int oy) override
	{
		x += ox;
		y += oy;
	}
};
