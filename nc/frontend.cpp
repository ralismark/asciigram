/**
 * \file
 * The main file of the NCurses frontend, which defines main().
 */

#include "cursor.hpp"
#include "globals.hpp"
#include "modes.hpp"
#include "renderer.hpp"

#include "../base.hpp"

#include <ncurses.h>

int main()
{
	cur.x = 0; cur.y = 1;

	CursesSetup cs;
	CursesRenderer crender;

	init_pair(10, COLOR_BLACK, COLOR_GREEN);
	init_pair(11, COLOR_WHITE, COLOR_RED);

	// 0: Universal
	ls.layers.emplace_back(std::make_unique<Universal>());

	// 1: Mode
	ls.layers.emplace_back();
	setmode(Mode::Normal);

	for(int input = ' '; true; input = getch()) {
		getmaxyx(stdscr, region.y, region.x);
		erase();

		ls.event(input);
		ls.frame();

		es.draw(crender);

		const char* mode_name = "???";

		switch(mode) {
		case Mode::Normal:
			mode_name = "Normal";
			break;
		case Mode::Visual:
			mode_name = "Visual";
			break;
		case Mode::Move:
			mode_name = "Move";
			break;
		case Mode::Box:
			mode_name = "Box";
			break;
		case Mode::Insert:
			mode_name = "Insert";
			break;
		case Mode::Arrow:
			mode_name = "Arrow";
			break;
		case Mode::Quit:
			// nothing needs to be done
			break;
		}

		attron(COLOR_PAIR(10));
		mvhline(0, 0, ' ', region.x);
		mvprintw(0, 1, "%d/%d -- %s -- '?' for help", 1 + idhere(), es.elements.size(), mode_name);

		auto clamp = [] (int val, int low, int high) { return val < low ? low : val > high ? high : val; };
		cur.y = clamp(cur.y, 1, region.y - 1);
		cur.x = clamp(cur.x, 0, region.x - 1);

		attroff(COLOR_PAIR(10));

		move(cur.y, cur.x);
		wnoutrefresh(stdscr);

		ls.post();

		doupdate();

		if(mode == Mode::Quit) {
			break;
		}
	}

}
