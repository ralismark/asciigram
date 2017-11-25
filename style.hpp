#pragma once

#include "base.hpp"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

struct Style
{
	std::vector<std::pair<char*, point>> get_display_points()
	{
		return {};
	}
};

// Manages a set of styles of a single style
template <typename T>
struct StyleManager
{
	std::vector<std::shared_ptr<T>> styles;
public:
	StyleManager()
		: styles(1, std::make_shared<T>()) // default first
	{
	}

	std::shared_ptr<T> get_first() const
	{
		return styles.front();
	}

	std::shared_ptr<T> duplicate_first()
	{
		styles.emplace_back(std::make_shared<T>(*this->get_first()));
		this->unshift();
	}

	std::vector<std::pair<char*, point>> get_display_points() const
	{
		return this->get_first()->get_display_points();
	}

	point get_display_range() const
	{
		point max = { 0, 0 };
		for(auto pair : this->get_display_points()) {
			max.x = std::max(max.x, pair.second.x);
			max.y = std::max(max.y, pair.second.y);
		}
		return max;
	}

	// make 2nd style be first
	void shift()
	{
		std::rotate(styles.begin(), styles.end() - 1, styles.end());
	}

	// make last style be first
	void unshift()
	{
		std::rotate(styles.begin(), styles.begin() + 1, styles.end());
	}
};
