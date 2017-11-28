#pragma once

/**
 * \file
 * This file defines the Canvas interface, which is a target to draw elements
 * to.
 */

#include "drawable.hpp"

#include <string>
#include <algorithm>

/**
 * An interface for a drawing target for Drawables.
 *
 * The derived class must implement impl_set (for setting a single character),
 * and can optionally also implement any of the other virtual impl_ method,
 * such as if there are more efficient ways to do so.
 *
 * Drawables can use most public methods for drawing themselves. These are
 * delegated to the corresponding virtual methods.
 */
struct Canvas
{
	/// Special characters when drawing
	enum Special : char
	{
		Transparent = '\0', ///< Is invisible (previous character is visible).
		Blank       = ' '   ///< Appears as blank.
	};

protected:

	/**
	 * Sets a single character \p fill at \p x and \p y.
	 *
	 * The implementer may assume that \p fill != '\0'.
	 */
	virtual void impl_set(char fill, int x, int y) = 0;

	/**
	 * Sets a vertical interval from (\p x, \p y1) to (\p x, \p y2)
	 * inclusive to \p fill.
	 *
	 * The implementer may assume that \p fill != '\0' and \p y1 <= \p y2.
	 */
	virtual void impl_linev(char fill, int x, int y1, int y2)
	{
		for(int y = y1; y <= y2; ++y) {
			this->set(fill, x, y);
		}
	}

	/**
	 * Sets a horizontal interval from (\p x1, \p y) to (\p x2, \p y)
	 * inclusive to \p fill.
	 *
	 * The implementer may assume that \p fill != '\0' and \p x1 <= \p x2.
	 */
	virtual void impl_lineh(char fill, int x1, int y, int x2)
	{
		for(int x = x1; x <= x2; ++x) {
			this->set(fill, x, y);
		}
	}

	/**
	 * Sets all characters in the rectangle (\p x1, \p y1) to (\p x2, \p
	 * y2) inclusive to \p fill.
	 *
	 * The implementer may assume that \p fill != '\0', \p x1 <= \p x2, and
	 * \p y1 <= \p y2
	 */
	virtual void impl_fill(char fill, int x1, int y1, int x2, int y2)
	{
		for(int y = y1; y <= y2; ++y) {
			this->impl_lineh(fill, x1, y, x2);
		}
	}

	/**
	 * Write a string \p str to the canvas, with the initial character at
	 * (\p x, \p y). Behaviour when \p str contains control characters
	 * (e.g. a newline) is unspecified. This includes the Transparent
	 * character.
	 *
	 * The implementer may assume that \p str is not empty.
	 */
	virtual void impl_direct(const std::string& str, int x, int y)
	{
		for(unsigned int idx = 0; idx < str.size(); ++idx) {
			this->impl_set(str[idx], x + idx, y);
		}
	}

public:

	virtual ~Canvas() = default;

	/**
	 * Set the character at (\p x, \p y) to \p fill.
	 */
	void set(char fill, int x, int y)
	{
		if(fill != Transparent) {
			this->impl_set(fill, x, y);
		}
	}

	/**
	 * Sets a vertical interval from (\p x, \p y1) to (\p x, \p y2)
	 * inclusive to \p fill.
	 */
	void linev(char fill, int x, int y1, int y2)
	{
		if(fill != Transparent) {
			this->impl_linev(fill, x, std::min(y1, y2), std::max(y1, y2));
		}
	}

	/**
	 * Sets a horizontal interval from (\p x1, \p y) to (\p x2, \p y)
	 * inclusive to \p fill.
	 */
	void lineh(char fill, int x1, int y, int x2)
	{
		if(fill != Transparent) {
			this->impl_lineh(fill, std::min(x1, x2), y, std::max(x1, x2));
		}
	}

	/**
	 * Sets all characters in the rectangle (\p x1, \p y1) to (\p x2, \p
	 * y2) inclusive to \p fill.
	 */
	void fill(char fill, int x1, int y1, int x2, int y2)
	{
		if(fill != Transparent) {
			this->impl_fill(fill, std::min(x1, x2), std::min(y1, y2),
				std::max(x1, x2), std::max(y1, y2));
		}
	}

	/**
	 * Write a string \p str to the canvas, with the initial character at
	 * (\p x, \p y). Behaviour when \p str contains control characters
	 * (e.g. a newline) is unspecified. This includes the Transparent
	 * character.
	 */
	void direct(const std::string& s, int x, int y)
	{
		if(!s.empty()) {
			this->impl_direct(s, x, y);
		}
	}

	/**
	 * Draw \p object onto the current canvas. This is the same as calling
	 * draw on \p object with *this as the argument.
	 */
	void draw(const Drawable& object)
	{
		object.draw(*this);
	}
};
