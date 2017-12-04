#pragma once

#include "drawable.hpp"
#include "fwd.hpp"

#include <iostream>

/// Interface to draw to
/// You need to implement impl_set, though can optionally implement any number
/// of the impl_* functions
struct Canvas
{
	/// Special characters when drawing
	enum Special : char
	{
		Transparent = '\0', /// Objects drawn with this char do not affect anythign
		Blank = ' ' /// Appears as blank
	};

protected:

	// implement: change a set corrdinate
	// assume: fill != '\0'
	virtual void impl_set(char fill, int x, int y) = 0;

	// implement: change a vertical interval
	// assume: fill != '\0', y1 < y2
	virtual void impl_linev(char fill, int x, int y1, int y2)
	{
		for(int y = y1; y <= y2; ++y) {
			this->set(fill, x, y);
		}
	}

	// implement: change a horizontal interval
	// assume: fill != '\0', x1 < x2
	virtual void impl_lineh(char fill, int x1, int y, int x2)
	{
		for(int x = x1; x <= x2; ++x) {
			this->set(fill, x, y);
		}
	}

	// implement: set all in an inclusive region
	// assume: fill != '\0', x1 < x2, y1 < y2
	virtual void impl_fill(char fill, int x1, int y1, int x2, int y2)
	{
		for(int y = y1; y <= y2; ++y) {
			this->impl_lineh(fill, x1, y, x2);
		}
	}

	// implement: write the string to the canvas starting at the set position
	// assume: str does not contain '\0'
	virtual void impl_direct(const std::string& str, int x, int y)
	{
		for(unsigned int idx = 0; idx < str.size(); ++idx) {
			this->impl_set(str[idx], x + idx, y);
		}
	}

public:

	virtual ~Canvas() = default;

	// set (x,y) to `x`
	void set(char fill, int x, int y)
	{
		if(fill != Transparent) {
			this->impl_set(fill, x, y);
		}
	}

	// draw a vertical line of `fill` from (x,y) to (x,y2) inclusive
	// ensure: y < y2
	void linev(char fill, int x, int y1, int y2)
	{
		if(fill != Transparent) {
			this->impl_linev(fill, x, std::min(y1, y2), std::max(y1, y2));
		}
	}

	// draw a horizontal line of `fill` from (x,y) to (x2,y) inclusive
	// ensure: x < x2
	void lineh(char fill, int x1, int y, int x2)
	{
		if(fill != Transparent) {
			this->impl_lineh(fill, std::min(x1, x2), y, std::max(x1, x2));
		}
	}

	// fill a region from (x1, y1) to (x2, y2) inclusive
	// ensure: x1 < x2, y1 < y2
	void fill(char fill, int x1, int y1, int x2, int y2)
	{
		if(fill != Transparent) {
			this->impl_fill(fill, std::min(x1, x2), std::min(y1, y2),
				std::max(x1, x2), std::max(y1, y2));
		}
	}

	// write a string s starting from (x, y)
	// ensure: s does not contain '\0'
	void direct(const std::string& s, int x, int y)
	{
		if(!s.empty()) {
			this->impl_direct(s, x, y);
		}
	}

	// draw a drawable onto itself
	void draw(const Drawable& object)
	{
		object.draw(*this);
	}
};
