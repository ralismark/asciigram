#pragma once

#include "clip.hpp"
#include "cursor.hpp"
#include "globals.hpp"
#include "layer.hpp"

#include "../item/box.hpp"
#include "../item/text.hpp"

#include <ncurses.h>
#include <cctype>
#include <algorithm>

enum class Mode {
	Normal,
	Move,
	Box,
	Insert,
	InsertBlock,

	Quit,
};

extern Mode mode;

void setmode(Mode m);

// change styles
struct StyleChangerLayer // {{{
	: public Layer
{
	StyleChanger sc;
	int part;
	char val;
public:
	StyleChangerLayer()
		: sc(sm.get_default())
		, part(0)
		, val(0)
	{
	}

	virtual bool event(int ev) override
	{
		if(part != 0) {
			if(isprint(ev)) {
				*sc.at_id(part, *sc.style) = ev;
			} else if(ev == KEY_BACKSPACE) {
				*sc.at_id(part, *sc.style) = Canvas::Transparent;
			} else {
				*sc.at_id(part, *sc.style) = val;
			}
			part = 0;
			return false;
		}

		switch(ev) {
		case 'q':
		case 's':
		case '\033':
			ls.layers.pop_back();
			break;
		case '1': case '2': case '3':
		case '4': case '5': case '6':
		case '7': case '8': case '9':
			part = ev - '0';
			val = *sc.at_id(part, *sc.style);
			*sc.at_id(part, *sc.style) = '#';
			break;
		case '+':
			sm.styles.emplace_back(std::make_shared<BoxStyle>(*sm.get_default()));
			break;
		case ']':
			std::rotate(sm.styles.begin(), sm.styles.begin() + 1, sm.styles.end());
			sc.style = sm.get_default();
			break;
		case '[':
			std::rotate(sm.styles.begin(), sm.styles.end() - 1, sm.styles.end());
			sc.style = sm.get_default();
			break;
		}
		return false;
	}

	virtual void post() override
	{
		sc.render();
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
		case '?':
			mvprintw(1, 0, "id here: %d", here);
			break;
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

