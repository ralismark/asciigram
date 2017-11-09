#pragma once

#include "../item/box.hpp"

#include <ncurses.h>

#include <vector>
#include <memory>

struct StyleChanger
{
	static char* at_id(int val, BoxStyle& parent)
	{
		switch(val) {
		case 1:
			return &parent.tl_corner;
		case 2:
			return &parent.tside;
		case 3:
			return &parent.tr_corner;
		case 4:
			return &parent.lside;
		case 5:
			return &parent.fill;
		case 6:
			return &parent.rside;
		case 7:
			return &parent.bl_corner;
		case 8:
			return &parent.bside;
		case 9:
			return &parent.br_corner;
		}
		return nullptr;
	}
public:
	std::shared_ptr<BoxStyle> style;
	WINDOW* win;
	int curs_save;
public:
	StyleChanger(std::shared_ptr<BoxStyle> style = std::make_shared<BoxStyle>())
		: style(style)
		, win(newwin(5, 20, 10, 10))
		, curs_save(curs_set(0))
	{
	}

	~StyleChanger()
	{
		delwin(win);
		curs_set(curs_save);
	}

	StyleChanger(StyleChanger&&) = delete;

	virtual void render()
	{
		auto unnull = [] (char& x) { x = x ? x : ' '; };
		BoxStyle nonull = *style;

		for(int i = 0; i < 9; ++i) {
			auto elem = *at_id(i + 1, nonull);
			char disp = elem ? elem : ' ';

			mvwaddch(win, 1 + i / 3, 7 + i % 3, disp);
		}

		mvwprintw(win, 1, 2, "123");
		mvwprintw(win, 2, 2, "456");
		mvwprintw(win, 3, 2, "789");

		box(win, 0, 0);
		wrefresh(win);
	}
};

struct StyleManager
{
	std::vector<std::shared_ptr<BoxStyle>> styles;
public:
	StyleManager()
	{
		auto default_style = std::make_shared<BoxStyle>();

		default_style->tside = '-';
		default_style->bside = '-';
		default_style->rside = '|';
		default_style->lside = '|';

		default_style->tl_corner = '.';
		default_style->tr_corner = '.';
		default_style->bl_corner = '\'';
		default_style->br_corner = '\'';

		styles.emplace_back(std::move(default_style));
	}

	std::shared_ptr<BoxStyle> get_default() const
	{
		return styles.front();
	}
};
