#include "cursor.hpp"
#include "globals.hpp"
#include "modes.hpp"
#include "renderer.hpp"

#include <ncurses.h>

int main()
{
	cur.x = 0; cur.y = 1;

	CursesSetup cs;
	CursesRenderer crender;

	// 0: Universal
	ls.layers.emplace_back(std::make_unique<Universal>());

	// 1: Mode
	ls.layers.emplace_back();
	setmode(Mode::Normal);

	for(int input = ' '; true; input = getch()) {
		coord region;
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
		case Mode::InsertBlock:
			mode_name = "InsertBlock";
			break;
		case Mode::Move:
			mode_name = "Move";
			break;
		}

		mvprintw(0, 0, "-- %s --", mode_name);

		auto clamp = [] (int val, int high) { return val < 0 ? 0 : val > high ? high : val; };

		move(clamp(cur.y, region.y - 1), clamp(cur.x, region.x - 1));
		refresh();

		ls.post();

		if(mode == Mode::Quit) {
			break;
		}
	}

}
