#pragma once

/**
 * \file
 * Define the Drawable interface for elements in a document, as well as a
 * container for a set of elements.
 */

#include <vector>
#include <memory>
#include <utility>

struct Canvas; // forward declare

/**
 * An interface for objects that can be drawn on a canvas.
 *
 * The derived class should implement the virtual methods to be fully
 * compatible with users of the interface. Most importantly, they should
 * implement draw(), which uses the Canvas interface to draw itself.
 *
 * A Drawable should only directly contain information related to the structure
 * of the object; styling should be done with types derived from Style, and the
 * Drawable containing a shared_ptr to one.
 */
struct Drawable
{
	virtual ~Drawable() = default;

	/**
	 * Allocate and create a copy of the object.
	 */
	virtual std::unique_ptr<Drawable> clone() const = 0;

	/**
	 * Draw the object to a Canvas \p canvas.
	 */
	virtual void draw(Canvas& canvas) const = 0;

	/**
	 * Move the entire object by (\p x, \p y).
	 */
	virtual void shift(int x, int y) = 0;
};

/**
 * A container for an ordered sequence of Drawable objects.
 *
 * This is technically an element as well (supporting most features), but is not intended to be used as such.
 */
struct ElementStack
	: public Drawable
{
	std::vector<std::unique_ptr<Drawable>> elements;
public:
	virtual void draw(Canvas& canvas) const override
	{
		for(auto& elem : elements) {
			elem->draw(canvas);
		}
	}

	virtual std::unique_ptr<Drawable> clone() const
	{
		std::vector<std::unique_ptr<Drawable>> elements_copy;
		for(auto& elem : elements) {
			elements_copy.push_back(elem->clone());
		}
		auto copy = std::make_unique<ElementStack>();
		copy->elements = std::move(elements_copy);
		return copy;
	}

	virtual void shift(int x, int y)
	{
		for(auto& elem : elements) {
			elem->shift(x, y);
		}
	}

	/**
	 * Construct and add a new type derived from Drawable.
	 */
	template <typename T, typename... Args>
	void add(Args&&... args)
	{
		auto elem = std::make_unique<T>(std::forward<Args>(args)...);
		elements.emplace_back(std::move(elem));
	}

	/**
	 * Get the top element, cast to a specific Drawable.
	 */
	template <typename T>
	T* back_as()
	{
		auto raw_ptr = elements.back().get();
		return dynamic_cast<T*>(raw_ptr);
	}
};
