#pragma once

#include "globals.hpp"
#include "layer.hpp"

#include <ncurses.h>

#include <sstream>
#include <string>

const std::vector<std::string>& helplines();

struct HelpLayer // {{{
	: public Layer
{
	WINDOW* win;
	int cursor_save; // Save cursor style
	int line;
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

	void scroll_up()
	{
		if(line > 0) {
			--line;
		}
	}

	void scroll_down()
	{
		if(line < helplines().size()) {
			++line;
		}
	}

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

	virtual void post() override
	{
		werase(win);
		box(win, 0, 0); // Standard border

		mvwaddstr(win, 1, 5, "q/ESC/?: close help    j/down: scroll down    h/up: scroll up");

		int width, height;
		getmaxyx(win, height, width);

		int printing_line = 2;
		for(int i = line; i < helplines().size() && printing_line < height - 1; ++i, ++printing_line) {
			mvwaddstr(win, printing_line, 2, helplines()[i].c_str());
		}

		mvwaddstr(win, 0, 2, " Help ");
		wnoutrefresh(win);
	}
};
