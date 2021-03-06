#pragma once

/**
 * \file
 * This file defines the various layers used in the programs, many of which are
 * also modes.
 *
 * The individual methods common to all layers are not documented - their
 * features are explained in the help message in help.cpp.
 *
 * When adding a mode, several things need to be done:
 * - Create mode class
 * - Add enumeration to Mode
 * - Handle the new enumeration in setmode() (in modes.cpp)
 * - Handle it in main() (in frontend.cpp) as well to set the displayed string
 * - Add way to enter mode (usually in Normal mode)
 * - Document the new mode as well as the key to enter it in the help message (help.cpp)
 */

#include "clip.hpp"
#include "cursor.hpp"
#include "globals.hpp"
#include "help.hpp"
#include "layer.hpp"

#include "../item/box.hpp"
#include "../item/text.hpp"
#include "../item/arrow.hpp"

#include "../asciirender.hpp"
#include "../sysclip.hpp"

#include <ncurses.h>
#include <cctype>
#include <algorithm>

/**
 * The various modes available in the program.
 *
 * With the exertion of the Quit pseudo-mode, these usually correspond to a
 * layer with the same name, but with "Mode" appended.
 */
enum class Mode {
	Normal,
	Visual,
	Move,
	Box,
	Insert,
	Arrow,

	Quit, ///< This pseudo-mode is used to indicate when to exit the program.
};

/**
 * The current mode.
 *
 * This is read-only, and should not be directly modified. To change the mode,
 * use setmode().
 */
extern Mode mode;

/**
 * Change the current mode, replacing the mode layer with the corresponding
 * mode.
 *
 * The old mode is destructed and the new one is constructed each time the mode
 * changes.
 */
void setmode(Mode m);

/**
 * A pop-up for changing the style, templated on the style type.
 *
 * This non-mode layer shows a pop-up that allows the user to change styles, as
 * well as duplicate them. This is used to style items differently.
 *
 * The pop-up has two sides, the left with placeholders, and the right with the
 * actual elements for styling. Pressing the corresponding placeholder, and
 * then the new style changes the corresponding part.
 *
 * More information about this is available in the help message.
 */
template <typename T>
struct StyleChangerLayer // {{{
	: public Layer
{
	/**
	 * The sequence of letters/numbers to use for placeholders.
	 */
	static constexpr const char* placeholders = "1234567890asdfghjklzxcvbnm";
public:
	WINDOW* win;
	int cursor_save; // Save cursor style

	int part_id; // picked part
	char save_part; // previous style
public:
	StyleChangerLayer()
		: win(nullptr)
		, cursor_save(curs_set(0)) // hide cursor
		, part_id(-1)
		, save_part(0)
	{
		auto max = msm.get<T>().get_display_range();
		win = newwin(max.y + 4, std::max(17, max.x * 2 + 11), 10, 10); // 17 for title
	}

	~StyleChangerLayer()
	{
		delwin(win);
		curs_set(cursor_save);
	}

	virtual bool event(int ev) override
	{
		auto display_points = msm.get<T>().get_display_points();

		if(part_id != -1) {
			char* part = display_points[part_id].first;
			if(isprint(ev)) {
				*part = ev;
			} else if(ev == KEY_BACKSPACE) {
				*part = Canvas::Transparent;
			} else {
				*part = save_part;
			}
			part_id = -1;
			return false;
		}

		std::string ph = placeholders;
		auto idx = ph.find(ev);
		if(idx != std::string::npos && idx < display_points.size()) {
			part_id = idx;
			char* part = display_points[part_id].first;
			save_part = *part;
			*part = '#';
			return false;
		}

		switch(ev) {
		case 'q':
		case '\033':
			ls.layers.pop_back();
			break;
		case '+':
			msm.get<T>().duplicate_first();
			break;
		case ']':
			msm.get<T>().unshift();
			break;
		case '[':
			msm.get<T>().shift();
			break;
		}
		return false;
	}

	virtual void post() override
	{
		werase(win);
		box(win, 0, 0); // standard border

		auto max = msm.get<T>().get_display_range();

		int idx = 0;
		for(auto& part : msm.get<T>().get_display_points()) {
			// placeholder
			mvwaddch(win, part.second.y + 1, part.second.x + 2, placeholders[idx]);

			// existing component
			if(*part.first != '\0') {
				mvwaddch(win, part.second.y + 1, part.second.x + max.x + 7, *part.first);
			}

			++idx;
		}

		// Draw split line
		mvwvline(win, 1, max.x + 5, ACS_VLINE, max.y + 2);

		mvwprintw(win, 0, 2, " Change style ");
		wnoutrefresh(win);
	}
}; // }}}

/**
 * Universally available actions, common to all modes.
 *
 * This layer exists as index 0, below the mode layer (which is index 1). This
 * layer handles basic movement functions, as well as ones useful across all
 * modes. This includes the help pop-up and returning to normal mode.
 */
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
		case KEY_LEFT:
			--cur.x;
			break;
		case 'j':
		case KEY_DOWN:
			++cur.y;
			break;
		case 'k':
		case KEY_UP:
			--cur.y;
			break;
		case 'l':
		case KEY_RIGHT:
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

/**
 * Basic mode for basic actions, such as entering other modes.
 *
 * This mode is for generic activity, such as basic element operations, and
 * nothing too specific (which should have a dedicated mode/layer). In
 * particular, this mode allows you to enter all other modes.
 */
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
		case 'y':
			clip.yank_here();
			break;
		case 'p':
			clip.paste_here();
			break;
		case 'v':
			setmode(Mode::Visual);
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
			ls.layers.emplace_back(std::make_unique<StyleChangerLayer<BoxStyle>>());
			break;
		case 'S':
			ls.layers.emplace_back(std::make_unique<StyleChangerLayer<ArrowStyle>>());
			break;
		case '<': // lower
			if(here > 0) {
				std::swap(es.elements[here], es.elements[here - 1]);
			}
			break;
		case '>': // higher
			if(here != -1 && here < static_cast<int>(es.elements.size()) - 1) {
				std::swap(es.elements[here], es.elements[here + 1]);
			}
			break;
		default:
			return true;
		}
		return false;
	}
}; // }}}

/**
 * Block selection mode, for working with multiple elements.
 *
 * The most notable feature here is the grouping functionality.
 */
struct VisualMode // {{{
	: public Layer
{
	point p1, p2;
public:
	VisualMode()
		: p1(cur), p2(cur)
	{
	}

	void shift(int x, int y) {
		p1.x += x;
		p1.y += y;

		p2.x += x;
		p2.y += y;
	}

	virtual bool event(int val) override
	{
		auto ids = id_in_region(p1.x, p1.y, p2.x, p2.y);

		auto make_group = [&] {
			ElementStack group;
			int offset = 0;
			for(auto& id : ids) { // ids are sorted
				auto it = es.elements.begin() + id + offset;
				if(auto* old_stack = dynamic_cast<ElementStack*>(it->get())) {
					// unpack the stack
					for(auto& elem : old_stack->elements) {
						group.elements.push_back(std::move(elem));
					}
				} else {
					group.elements.push_back(std::move(*it));
				}
				es.elements.erase(it);
				--offset;
			}
			ids.clear(); // ids are invalid
			return group;
		};

		auto copy_group = [&] {
			ElementStack group;
			for(auto& id : ids) { // ids are sorted
				auto it = es.elements.begin() + id;
				if(auto* old_stack = dynamic_cast<ElementStack*>(it->get())) {
					// unpack the stack
					for(auto& elem : old_stack->elements) {
						group.elements.push_back(elem->clone());
					}
				} else {
					group.elements.push_back((*it)->clone());
				}
			}
			return group;
		};

		// not a visual operation - propagate
		bool more = false;

		switch(val) {
		case 'v':
			break;
		case 'o':
			std::swap(p1, p2);
			return false;
		case 'O':
			std::swap(p1.x, p2.y);
			return false;
		case 'g':
			es.elements.push_back(std::make_unique<ElementStack>(make_group()));
			break;
		case 'y':
			clip.contents = std::make_unique<ElementStack>(copy_group());
			clip.x = cur.x;
			clip.y = cur.y;
			break;
		case 'c':
			{
				AsciiRenderer ar{p1.x, p1.y, p2.x, p2.y};
				es.draw(ar);
				copy_to_sysclip(ar.joined());
			} break;
		case 'x':
			make_group(); // destruct to kill
			break;
		default:
			more = true;
		}

		if(!more) {
			setmode(Mode::Normal);
			return false;
		}

		switch(val) {
		case 'H':
			this->shift(1, 0);
			break;
		case 'J':
			this->shift(0, -1);
			break;
		case 'K':
			this->shift(0, 1);
			break;
		case 'L':
			this->shift(-1, 0);
			break;
		}
		return true;
	}

	virtual void frame() override
	{
		p2 = cur;
	}

	virtual void post() override
	{
		point min{ std::min(p1.x, p2.x), std::min(p1.y, p2.y) };
		point max{ std::max(p1.x, p2.x), std::max(p1.y, p2.y) };

		int width = max.x - min.x + 1;

		for(int y = min.y; y <= max.y; ++y) {
			mvchgat(y, min.x, width, WA_NORMAL, 11, nullptr);
		}

		// redo this manually, since this would break dialogs if moved after
		move(cur.y, cur.x);
		wnoutrefresh(stdscr);
	}
}; // }}}

/**
 * Move a particular element.
 */
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
			case KEY_LEFT:
				es.elements[id]->shift(-1, 0);
				break;
			case 'j':
			case KEY_DOWN:
				es.elements[id]->shift(0, 1);
				break;
			case 'k':
			case KEY_UP:
				es.elements[id]->shift(0, -1);
				break;
			case 'l':
			case KEY_RIGHT:
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

/**
 * Draw a box, from the starting position to the cursor.
 */
struct BoxMode // {{{
	: public Layer
{
	BoxMode()
	{
		es.add<Box>(cur.x, cur.y, msm.get<BoxStyle>().get_first());
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

/**
 * Insert a block of (raw) text.
 */
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
			if(!text.string.empty()) {
				auto back = text.string.back();
				text.string.pop_back();
				if(back == '\n') {
					--cur.y;
					cur.x = text.x;
					for(auto it = text.string.rbegin(); it != text.string.rend() && *it != '\n'; ++it) {
						++cur.x;
					}
				} else {
					--cur.x;
				}
			}
			break;
		default:
			return true;
		}
		return false;
	}
}; // }}}

/**
 * Draw an arrow.
 */
struct ArrowMode // {{{
	: public Layer
{
	ArrowMode()
	{
		es.add<Arrow>(cur.x, cur.y, msm.get<ArrowStyle>().get_first());
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
