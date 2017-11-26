#pragma once

#include "clip.hpp"
#include "cursor.hpp"
#include "globals.hpp"
#include "help.hpp"
#include "layer.hpp"

#include "../item/box.hpp"
#include "../item/text.hpp"
#include "../item/arrow.hpp"

#include <ncurses.h>
#include <cctype>
#include <algorithm>

enum class Mode {
	Normal,
	Move,
	Box,
	Insert,
	Arrow,

	Quit,
};

extern Mode mode;

void setmode(Mode m);

// change styles
struct StyleChangerLayer // {{{
	: public Layer
{
	WINDOW* win;
	int cursor_save; // Save cursor style
	int part_id; // picked part
	char save_part; // previous style
public:
	StyleChangerLayer()
		: win(newwin(5, 15, 10, 10))
		, cursor_save(curs_set(0)) // hide cursor
		, part_id(0)
		, save_part(0)
	{
	}

	~StyleChangerLayer()
	{
		delwin(win);
		curs_set(cursor_save);
	}

	char* at_id(int val) const
	{
		BoxStyle& style = *sm.get_default();

		char* refs[] = {
			&style.tl_corner,
			&style.tside,
			&style.tr_corner,
			&style.lside,
			&style.fill,
			&style.rside,
			&style.bl_corner,
			&style.bside,
			&style.br_corner
		};

		if(1 <= val && val <= 9) {
			return refs[val - 1];
		}
		return nullptr;
	}

	virtual bool event(int ev) override
	{
		if(part_id != 0) {
			if(isprint(ev)) {
				*this->at_id(part_id) = ev;
			} else if(ev == KEY_BACKSPACE) {
				*this->at_id(part_id) = Canvas::Transparent;
			} else {
				*this->at_id(part_id) = save_part;
			}
			part_id = 0;
			return false;
		}

		switch(ev) {
		case 'q':
		case 's':
		case '\033':
			ls.layers.pop_back(); // quit
			break;
		case '1': case '2': case '3':
		case '4': case '5': case '6':
		case '7': case '8': case '9':
			part_id = ev - '0';
			save_part = *this->at_id(part_id);
			*this->at_id(part_id) = '#';
			break;
		case '+':
			// copy current
			sm.styles.emplace_back(std::make_shared<BoxStyle>(*sm.get_default()));
			break;
		case ']':
			std::rotate(sm.styles.begin(), sm.styles.begin() + 1, sm.styles.end());
			break;
		case '[':
			std::rotate(sm.styles.begin(), sm.styles.end() - 1, sm.styles.end());
			break;
		}
		return false;
	}

	virtual void post() override
	{
		box(win, 0, 0); // standard border

		for(int i = 0; i < 9; ++i) {
			auto elem = *this->at_id(i + 1);
			mvwaddch(win, 1 + (i / 3), 7 + (i % 3), elem ? elem : ' ');
		}

		mvwprintw(win, 1, 2, "123");
		mvwprintw(win, 2, 2, "456");
		mvwprintw(win, 3, 2, "789");

		mvwprintw(win, 0, 2, " Set style ");
		wnoutrefresh(win);
	}
}; // }}}

// Default response to actions
struct Universal // {{{
	: public Layer
{
	virtual bool event(int val) override
	{
		switch(val) {
		case '\033': // escape to normal
			setmode(Mode::Normal);
			break;
		case '?':
			ls.layers.emplace_back(std::make_unique<HelpLayer>());
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
			mvprintw(1, 0, "key %x", val);
			return true;
		}
		return false;
	}
}; // }}}

struct NormalMode // {{{
	: public Layer
{
	virtual bool event(int val) override
	{
		int here = idhere();
		switch(val) {
		case 'x':
			clip.remove_here();
			break;
		case 'p':
			clip.paste_here();
			break;
		case 'b':
			setmode(Mode::Box);
			break;
		case 'i':
			setmode(Mode::Insert);
			break;
		case 'a':
			setmode(Mode::Arrow);
			break;
		case 'm':
			if(here != -1) {
				setmode(Mode::Move);
			}
			break;
		case 's':
			ls.layers.emplace_back(std::make_unique<StyleChangerLayer>());
			break;
		case '<': // lower
			if(here > 0) {
				std::swap(es.elements[here], es.elements[here - 1]);
			}
			break;
		case '>': // higher
			if(here != -1 && here < es.elements.size() - 1) {
				std::swap(es.elements[here], es.elements[here + 1]);
			}
			break;
		default:
			return true;
		}
		return false;
	}
}; // }}}

struct MoveMode // {{{
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
}; // }}}

// drawing a box
struct BoxMode // {{{
	: public Layer
{
	BoxMode()
	{
		es.add<Box>(cur.x, cur.y, sm.get_default());
	}

	virtual bool event(int val) override
	{
		Box& box = *es.back_as<Box>();

		switch(val) {
		case 'x':
			clip.remove_here();
			setmode(Mode::Normal);
			break;
		case 'b':
			setmode(Mode::Normal);
			break;
		case 'o': // go to Other end
			std::swap(box.x1, box.x2);
			std::swap(box.y1, box.y2);
			cur.x = box.x2;
			cur.y = box.y2;
			break;
		case 'O':
			std::swap(box.x1, box.x2);
			cur.x = box.x2;
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
}; // }}}

struct InsertMode // {{{
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
		Text& text = *es.back_as<Text>();

		if(isprint(val)) {
			text.string.push_back(val);
			++cur.x;
		} else switch(val) {
		case '\r': case '\n':
			text.string.push_back('\n');
			cur.x = text.x;
			++cur.y;
			break;
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
}; // }}}

struct ArrowMode // {{{
	: public Layer
{
	ArrowMode()
	{
		es.add<Arrow>(cur.x, cur.y);
		es.back_as<Arrow>()->add_point(cur.x, cur.y);
	}

	virtual bool event(int val) override
	{
		Arrow& arrow = *es.back_as<Arrow>();

		point* second_last = nullptr;
		if(arrow.points.size() >= 2) {
			second_last = &(arrow.points.rbegin() + 1)->first;
		}

		switch(val) {
		case 'x':
			clip.remove_here();
			setmode(Mode::Normal);
			break;
		case 'a':
			if(second_last && cur.x == second_last->x && cur.y == second_last->y) {
				setmode(Mode::Normal);
			} else {
				arrow.add_point(cur.x, cur.y);
			}
			break;
		case 'o':
			arrow.flip_last();
			break;
		default:
			return true;
		}
		return false;
	}

	virtual void frame() override
	{
		Arrow& arrow = *es.back_as<Arrow>();
		arrow.points.back().first.x = cur.x;
		arrow.points.back().first.y = cur.y;
	}
}; // }}}
