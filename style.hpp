#pragma once

/**
 * \file
 * This file defines the Style interface (for styling Drawables), as well as a
 * template class StyleManager to manage them.
 */

#include "base.hpp"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

/**
 * An interface for styles for Drawable.
 *
 * This specifies which characters to draw a Drawable with. This is handled
 * throughout the project using shared_ptrs, which are managed by a
 * StyleManager.
 *
 * Derived types should have the default constructor initialize the style to
 * the default configuration (not empty/null/unspecified).
 */
struct Style
{
	/**
	 * Define a set of style part and their corresponding display position,
	 * used for the style popup dialog.  The char* does not need to be
	 * unique, though point does. The top-left corner (which contains the
	 * first item) should be (1, 1).
	 */
	std::vector<std::pair<char*, point>> get_display_points()
	{
		return {};
	}
};

/**
 * A manager for several styles for a single Drawable.
 *
 * This contains several styles of a single type. This class also provides
 * several other methods which are useful when developing a graphical
 * interface.
 */
template <typename T>
struct StyleManager
{
	std::vector<std::shared_ptr<T>> styles;
public:
	/**
	 * Constructs the object with one style, the default style.
	 */
	StyleManager()
		: styles(1, std::make_shared<T>()) // default first
	{
	}

	/**
	 * Return the first style in the set.
	 *
	 * The first style is canonically the default one, which is used for
	 * new Drawables.
	 */
	std::shared_ptr<T> get_first() const
	{
		return styles.front();
	}

	/**
	 * Duplicate the first style and put the copy at the front.
	 *
	 * This copies the first (default) style and allows that to be modified
	 * separately.
	 */
	void duplicate_first()
	{
		styles.emplace_back(std::make_shared<T>(*this->get_first()));
		this->unshift();
	}

	/**
	 * Return the display point mapping for the first style.
	 *
	 * This simply calls Style::get_display_points() on the first style.
	 */
	std::vector<std::pair<char*, point>> get_display_points() const
	{
		return this->get_first()->get_display_points();
	}

	/**
	 * Returns the maximum x and y values for the display range.
	 *
	 * This finds the upper limit of the points, to determine the space
	 * needed to draw the item.
	 */
	point get_display_range() const
	{
		point max = { 0, 0 };
		for(auto pair : this->get_display_points()) {
			max.x = std::max(max.x, pair.second.x);
			max.y = std::max(max.y, pair.second.y);
		}
		return max;
	}

	/**
	 * Rotate the styles to make the 2nd be first.
	 */
	void shift()
	{
		std::rotate(styles.begin(), styles.end() - 1, styles.end());
	}

	/**
	 * Rotate the styles to make the last be first.
	 */
	void unshift()
	{
		std::rotate(styles.begin(), styles.begin() + 1, styles.end());
	}
};
