#pragma once

/**
 * \file
 * This file defines the help layer, which shows a popup with help for
 * Asciigram.
 */

#include "cursor.hpp"
#include "globals.hpp"
#include "layer.hpp"

#include <ncurses.h>

#include <sstream>
#include <string>

/**
 * Get the lines of the help message. The value is computed (split from the raw
 * help message) only once - when this function is first called.
 *
 * The original help message is availabe in help.cpp.
 */
const std::vector<std::string>& helplines();

/**
 * Help dialog overlay. This provides a reference for all the available actions
 * in Asciigram, as well as an introduction.
 */
struct HelpLayer // {{{
	: public Layer
{
	WINDOW* win;
	int cursor_save; // Save cursor style
	unsigned int line;
public:
	HelpLayer()
		: win(newwin(region.y - 20, 100, 10, 10))
		, cursor_save(curs_set(0)) // hide cursor
		, line(0)
	{
	}

	~HelpLayer()
	{
		delwin(win);
		curs_set(cursor_save);
	}

	/**
	 * Scroll up, showing previous lines. This stops at the top, never
	 * going over.
	 */
	void scroll_up()
	{
		if(line > 0) {
			--line;
		}
	}

	/**
	 * Scroll down, showing more lines. This permits scrolling the bottom
	 * past the end, only stopping when there's one line left.
	 */
	void scroll_down()
	{
		if(line < helplines().size()) {
			++line;
		}
	}

	/**
	 * Handle events. The exact operations should be documeted in the help.
	 */
	virtual bool event(int ev) override
	{
		switch(ev) {
		case 'q':
		case '\033':
		case '?':
			ls.layers.pop_back(); // close help
			break;
		case 'j':
		case KEY_DOWN:
			this->scroll_down();
			break;
		case 'k':
		case KEY_UP:
			this->scroll_up();
			break;
		}
		return false;
	}

	/**
	 * Render the dialog box. This is done after element rendering, showing
	 * the box over everything else.
	 */
	virtual void post() override
	{
		werase(win);
		box(win, 0, 0); // Standard border

		mvwaddstr(win, 1, 5, "q/ESC/?: close help    j/down: scroll down    h/up: scroll up");

		// window size
		int width, height;
		getmaxyx(win, height, width);

		// make sure drawn text is inside window
		int printing_line = 2;
		for(unsigned int i = line; i < helplines().size() && printing_line < height - 1; ++i, ++printing_line) {
			mvwaddstr(win, printing_line, 2, helplines()[i].c_str());
		}

		// title
		mvwaddstr(win, 0, 2, " Help ");

		// apply drawn changes
		wnoutrefresh(win);
	}
};
