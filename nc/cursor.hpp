#pragma once

/**
 * \file
 * This file defines several things to help with using the cursor position.
 */

#include "globals.hpp"

#include "../canvas.hpp"
#include "../base.hpp"

#include <set>

/**
 * The current position of the cursor. Modifying this will move the cursor at
 * the end of the frame.
 */
extern point cur;

/**
 * The size of the window. This is a read-only variable; modifying this will
 * not modify the size of the terminal.
 */
extern point region;

/**
 * Get the index of the element under the cursor, returning -1 if nothing.
 *
 * This is done by determining the topmost element being drawn at the cursor
 * position.
 */
int idhere();

/**
 * Get all elements that are in a rectange from (\p x1, \p y1) to (\p x2, \p
 * y2) inclusive. They do not have to be in a specific order (e.g. x1 can be
 * greater than x2).
 *
 * Similar to idhere(), this determines the elements by what is drawm.
 */
std::set<int> id_in_region(int x1, int y1, int x2, int y2);
