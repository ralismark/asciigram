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

	// 0: Universal
	ls.layers.emplace_back(std::make_unique<Universal>());

	// 1: Mode
	ls.layers.emplace_back();
	setmode(Mode::Normal);

	for(int input = ' '; true; input = getch()) {
		point region;
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
		case Mode::Box:
			mode_name = "Box";
			break;
		case Mode::Insert:
			mode_name = "Insert";
			break;
		case Mode::Move:
			mode_name = "Move";
			break;
		case Mode::Arrow:
			mode_name = "Arrow";
			break;
		}

		attron(COLOR_PAIR(10));
		mvhline(0, 0, ' ', region.x);
		mvprintw(0, 1, "%d/%d -- %s --", 1 + idhere(), es.elements.size(), mode_name);

		auto clamp = [] (int val, int low, int high) { return val < low ? low : val > high ? high : val; };
		cur.y = clamp(cur.y, 1, region.y - 1);
		cur.x = clamp(cur.x, 0, region.x - 1);

		attroff(COLOR_PAIR(10));

		move(cur.y, cur.x);
		refresh();

		ls.post();

		if(mode == Mode::Quit) {
			break;
		}
	}

}
