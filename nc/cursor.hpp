#pragma once

#include "globals.hpp"

#include "../canvas.hpp"
#include "../base.hpp"

#include <set>

extern point cur;
extern point region;

int idhere();
std::set<int> id_in_region(int x1, int y1, int x2, int y2);
