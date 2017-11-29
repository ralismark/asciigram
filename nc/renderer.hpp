#pragma once

#include "../canvas.hpp"

#include <ncurses.h>

// RAII object for curses initialisation, config and finalisation
struct CursesSetup
{
	CursesSetup()
	{
		initscr();
		start_color();

		cbreak();
		keypad(stdscr, true);
		noecho();
		ESCDELAY = 100;
	}

	~CursesSetup()
	{
		endwin();
	}
};

// Draw elements as curses
struct CursesRenderer
	: public Canvas
{
public:
	virtual void impl_set(char fill, int x, int y) override
	{
		mvaddch(y, x, fill);
	}

	virtual void impl_linev(char fill, int x, int y1, int y2) override
	{
		mvvline(y1, x, fill, y2 - y1 + 1);
	}

	virtual void impl_lineh(char fill, int x1, int y, int x2) override
	{
		mvhline(y, x1, fill, x2 - x1 + 1);
	}

	virtual void impl_direct(const std::string& str, int x, int y) override
	{
		mvprintw(y, x, "%s", str.c_str());
	}
};
