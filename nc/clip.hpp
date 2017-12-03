#pragma once

/**
 * \file
 * This file defines Register, a class which acts as a clipboard.
 */

#include "globals.hpp"
#include "cursor.hpp"

// like vim registers, for copying and pasting
/**
 * A container for a single Drawable, with operations for a clipboard. Offset
 * from the cursor is maintained, so pasting will correctly move the element.
 *
 * The naming comes from Vim registers, which are their equivalent of clipboards.
 */
struct Register
{
	std::unique_ptr<Drawable> contents;
	int x, y; // original positions
public:
	/**
	 * Remove the element currently under the cursor into the register.
	 */
	void remove_here()
	{
		int id = idhere();
		if(id == -1) {
			return;
		}

		contents = std::move(es.elements[id]);
		es.elements.erase(es.elements.begin() + id);
		x = cur.x;
		y = cur.y;
	}

	/**
	 * Clone the element under the cursor, putting the copy into the clipboard.
	 */
	void yank_here()
	{
		int id = idhere();
		if(id == -1) {
			return;
		}

		contents = es.elements[id]->clone();
		x = cur.x;
		y = cur.y;
	}

	/**
	 * Paste the item currently in the clipboard, fixing up the offset from
	 * the cursor.
	 */
	void paste_here()
	{
		if(contents) {
			es.elements.emplace_back(contents->clone());
			es.elements.back()->shift(cur.x - x, cur.y - y);
		}
	}
};

/**
 * The main clipboard used in the program.
 */
extern Register clip;
