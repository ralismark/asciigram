#pragma once

/**
 * \file
 * This file defines the classes used to interact with NCurses for a major
 * portion of the program, such as rendering.
 */

#include "../canvas.hpp"
#include "../sysclip.cpp"

#include <ncurses.h>

/**
 * RAII object for NCurses initialisation and shutdown.
 *
 * A single instance of this object should be made in main(), then left there.
 * The constructor and destructor does everything here.
 */
struct CursesSetup
{
	CursesSetup()
	{
		initscr();
		start_color();
		use_default_colors();

		cbreak();
		keypad(stdscr, true);
		noecho();
		ESCDELAY = 100;
	}

	~CursesSetup()
	{
		endwin();
		deinit_sysclip();
	}
};

/**
 * NCurses rendering of elements.
 *
 * This is an implementation of a Canvas that uses the NCurses API to draw
 * elements.
 */
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
