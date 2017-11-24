#pragma once

struct point
{
	int x, y;
public:
	point() = default;

	point(int x, int y)
		: x(x), y(y)
	{
	}
};
