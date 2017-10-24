#include "renderer.hpp"
#include "styleman.hpp"

#include "../elem_stack.hpp"
#include "../item/box.hpp"
#include "../item/text.hpp"

#include <ncurses.h>

#include <functional>
#include <map>

struct OwnerFinder // {{{
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
}; // }}}

enum class Mode {
	Normal,
	Box
} mode = Mode::Normal;
bool quit = false;

StyleManager sm;
ElementStack es;
CursesRenderer crender;

struct {
	int x, y;
} cur;

int idhere()
{
	OwnerFinder of(cur.x, cur.y);
	for(int i = 0; i < es.elements.size(); ++i) {
		of.current_id = i;
		of.draw(*es.elements[i]);
	}
	return of.target_id;
}

auto setmode_f(Mode m)
{
	return [m] { mode = m; };
}

template <typename F>
auto ifhere_f(F fn)
{
	return [fn] {
		int id = idhere();
		if(id != -1) {
			fn(id);
		}
	};
}

std::map<int, std::function<void()>> box_km()
{
	std::map<int, std::function<void()>> km;

	km[0] = [] {
		auto box = es.back_as<Box>();
		box->x2 = cur.x;
		box->y2 = cur.y;
	};

	km['\033'] = setmode_f(Mode::Normal);

	km['h'] = [] { --cur.x; };
	km['j'] = [] { ++cur.y; };
	km['k'] = [] { --cur.y; };
	km['l'] = [] { ++cur.x; };

	km['x'] = [] { mode = Mode::Normal; es.elements.pop_back(); };

	km['b'] = setmode_f(Mode::Normal);

	return km;
}

std::map<int, std::function<void()>> normal_km()
{
	std::map<int, std::function<void()>> km;

	km['q'] = [] { quit = true; };

	km['h'] = [] { --cur.x; };
	km['j'] = [] { ++cur.y; };
	km['k'] = [] { --cur.y; };
	km['l'] = [] { ++cur.x; };

	km['?'] = ifhere_f([] (int id) { mvprintw(1, 0, "Id %d"); });
	km['x'] = ifhere_f([] (int id) { es.elements.erase(es.elements.begin() + id); });
	km['b'] = [] { mode = Mode::Box; es.add<Box>(cur.x, cur.y, sm.get_default()); };

	return km;
}

int main()
{
	CursesSetup cs;

	std::map<Mode, std::map<int, std::function<void()>>> keymaps;
	keymaps[Mode::Normal] = normal_km();
	keymaps[Mode::Box] = box_km();

	for(int input = ' '; true; input = getch()) {
		getyx(stdscr, cur.y, cur.x);
		erase();

		if(keymaps[mode].count(input)) {
			keymaps[mode].at(input)();
		}

		if(keymaps[mode].count(0)) {
			keymaps[mode].at(0)();
		}

		es.draw(crender);

		const char* mode_names[] = { "Normal", "Box" };
		mvprintw(0, 0, "-- %s --", mode_names[static_cast<int>(mode)]);

		move(cur.y, cur.x);
		refresh();

		if(quit) {
			break;
		}
	}

}
