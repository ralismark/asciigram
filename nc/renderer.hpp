#pragma once

#include "../drawable.hpp"
#include "../canvas.hpp"
#include "../item/box.hpp" // Avoid ncurese collision

#include <ncurses.h>

struct CursesSetup
{
	CursesSetup()
	{
		initscr();

		cbreak();
		keypad(stdscr, true);
		noecho();
	}

	~CursesSetup()
	{
		endwin();
	}
};

struct CursesRenderer
	: public Canvas
{
public:
	virtual void impl_set(char fill, int x, int y) override
	{
		mvaddch(y, x, fill);
	}

	virtual void impl_vline(char fill, int x, int y1, int y2) override
	{
		mvvline(y1, x, fill, y2 - y1);
	}

	virtual void impl_hline(char fill, int x1, int y, int x2) override
	{
		mvhline(y, x1, fill, x2 - x1);
	}
};
