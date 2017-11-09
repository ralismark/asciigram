#include "layer.hpp"
#include "renderer.hpp"
#include "styleman.hpp"

#include "../elem_stack.hpp"
#include "../item/box.hpp"
#include "../item/text.hpp"

#include <ncurses.h>
#include <cctype>

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
	Move,
	Box,
	Insert,
	InsertBlock,

	Quit,
} mode = Mode::Normal;

StyleManager sm;
ElementStack es;
CursesRenderer crender;
LayerStack ls;

struct {
	int x, y;
} cur, region;

int idhere()
{
	OwnerFinder of(cur.x, cur.y);
	for(int i = 0; i < es.elements.size(); ++i) {
		of.current_id = i;
		of.draw(*es.elements[i]);
	}
	return of.target_id;
}

// wrapper for when we need to actually switch around layers
void setmode(Mode m);

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

// Default response to actions
struct Universal
	: public Layer
{
	virtual bool event(int val) override
	{
		switch(val) {
		case '\033': // escape to normal
			setmode(Mode::Normal);
			break;
		case 'h':
			--cur.x;
			break;
		case 'j':
			++cur.y;
			break;
		case 'k':
			--cur.y;
			break;
		case 'l':
			++cur.x;
			break;
		case 'H':
			es.shift(1, 0);
			++cur.x;
			break;
		case 'J':
			es.shift(0, -1);
			--cur.y;
			break;
		case 'K':
			es.shift(0, 1);
			++cur.y;
			break;
		case 'L':
			es.shift(-1, 0);
			--cur.x;
			break;
		case 'q':
			setmode(Mode::Quit);
			break;
		default:
			mvprintw(1, 0, "key %d", val);
			return true;
		}
		return false;
	}
};

struct NormalMode
	: public Layer
{
	virtual bool event(int val) override
	{
		int here = idhere();
		switch(val) {
		case '?':
			if(here != -1) {
				mvprintw(1, 0, "id here: %d", here);
			}
			break;
		case 'x':
			if(here != -1) {
				es.elements.erase(es.elements.begin() + here);
			}
			break;
		case 'b':
			setmode(Mode::Box);
			break;
		case 'i':
			setmode(Mode::Insert);
			break;
		case 'm':
			if(here != -1) {
				setmode(Mode::Move);
			}
			break;
		default:
			return true;
		}
		return false;
	}
};

struct MoveMode
	: public Layer
{
	int id;
public:
	MoveMode()
		: id(idhere())
	{
	}

	virtual bool event(int val) override
	{
		if(id != -1) {
			switch(val) {
			case 'h':
				es.elements[id]->shift(-1, 0);
				break;
			case 'j':
				es.elements[id]->shift(0, 1);
				break;
			case 'k':
				es.elements[id]->shift(0, -1);
				break;
			case 'l':
				es.elements[id]->shift(1, 0);
				break;
			}
		}
		if(val == 'm') {
			setmode(Mode::Normal);
			return false;
		}
		return true;
	}
};

// drawing a box
struct BoxMode
	: public Layer
{
	BoxMode()
	{
		es.add<Box>(cur.x, cur.y, sm.get_default());
	}

	virtual bool event(int val) override
	{
		switch(val) {
		case 'x':
			es.elements.pop_back();
			setmode(Mode::Normal);
			break;
		case 'b':
			setmode(Mode::Normal);
			break;
		default:
			return true;
		}
		return false;
	}

	virtual void frame()
	{
		auto box = es.back_as<Box>();
		box->x2 = cur.x;
		box->y2 = cur.y;
	}
};

struct InsertMode
	: public Layer
{
	InsertMode()
	{
		// TODO: re-inherit and edit existing value
		es.add<Text>(cur.x, cur.y);
	}

	~InsertMode()
	{
		if(es.back_as<Text>()->string.empty()) {
			es.elements.pop_back();
		}
	}

	virtual bool event(int val) override
	{
		if(isprint(val)) {
			es.back_as<Text>()->string.push_back(val);
			++cur.x;
			return false;
		}
		switch(val) {
		case KEY_BACKSPACE:
			if(!es.back_as<Text>()->string.empty()) {
				es.back_as<Text>()->string.pop_back();
				--cur.x;
			}
			break;
		default:
			return true;
		}
		return false;
	}
};

// wrapper for when we need to actually switch around layers
void setmode(Mode m)
{
	mode = m;
	ls.layers[1].reset();

	switch(mode) {
	case Mode::Normal:
		ls.layers[1] = std::make_unique<NormalMode>();
		break;
	case Mode::Box:
		ls.layers[1] = std::make_unique<BoxMode>();
		break;
	case Mode::Insert:
		ls.layers[1] = std::make_unique<InsertMode>();
		break;
	case Mode::Move:
		ls.layers[1] = std::make_unique<MoveMode>();
		break;
	case Mode::Quit:
		break;
	default:
		setmode(Mode::Normal);
	}
}

int main()
{
	cur.x = 0; cur.y = 1;

	CursesSetup cs;

	// 0: Universal
	ls.layers.emplace_back(std::make_unique<Universal>());

	// 1: Mode
	ls.layers.emplace_back();
	setmode(Mode::Normal);

	for(int input = ' '; true; input = getch()) {
		getmaxyx(stdscr, region.y, region.x);
		erase();
		--region.y; --region.x;

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

		move(clamp(cur.y, region.y), clamp(cur.x, region.x));
		refresh();

		if(mode == Mode::Quit) {
			break;
		}
	}

}
