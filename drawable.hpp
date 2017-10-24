#pragma once

#include "fwd.hpp"

/// Something that can be drawn on a canvas
struct Drawable
{
	virtual ~Drawable() = default;

	virtual void draw(Canvas& canvas) const = 0;
};
