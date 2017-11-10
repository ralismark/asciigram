#pragma once

#include "globals.hpp"

#include "../canvas.hpp"

struct coord
{
	int x, y;
};

extern coord cur;

struct OwnerFinder
	: public Canvas
{
	int current_id;
	int target_id;
	int tx, ty;
public:
	OwnerFinder(int x, int y)
		: current_id(-1), target_id(-1), tx(x), ty(y)
	{
	}

	virtual void impl_set(char fill, int x, int y) override
	{
		if(x == tx && y == ty) {
			target_id = current_id;
		}
	}
};

int idhere();
