#pragma once

#include "globals.hpp"
#include "cursor.hpp"

// like vim registers, for copying and pasting
struct Register
{
	std::unique_ptr<Drawable> contents;
	int x, y; // original positions
public:
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

	void paste_here()
	{
		if(contents) {
			es.elements.emplace_back(contents->clone());
			es.elements.back()->shift(cur.x - x, cur.y - y);
		}
	}
};

extern Register clip;
