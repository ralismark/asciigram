#pragma once

/**
 * \file
 * This file defines generic types widely used throughout the project.
 */

/**
 * Stores a coordinate pair of integers.
 *
 * This type exists to prevent the duplication of similar types throughout the
 * project (e.g. representing cursor location, position of element, etc.).
 */
struct point
{
	int x, y;
public:
	point() = default;

	point(int x, int y)
		: x(x), y(y)
	{
	}
};
