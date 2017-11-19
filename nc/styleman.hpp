#pragma once

#include "../item/box.hpp"

#include <ncurses.h>

#include <vector>
#include <memory>

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
