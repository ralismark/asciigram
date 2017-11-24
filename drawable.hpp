#pragma once

#include "fwd.hpp"

#include <vector>
#include <memory>
#include <utility>

/// Something that can be drawn on a canvas
struct Drawable
{
	virtual ~Drawable() = default;

	virtual void draw(Canvas& canvas) const = 0;
	virtual void shift(int x, int y) = 0;
};

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

	virtual void shift(int x, int y)
	{
		for(auto& elem : elements) {
			elem->shift(x, y);
		}
	}

	template <typename T, typename... Args>
	void add(Args&&... args)
	{
		auto elem = std::make_unique<T>(std::forward<Args>(args)...);
		elements.emplace_back(std::move(elem));
	}

	template <typename T>
	T* back_as()
	{
		auto raw_ptr = elements.back().get();
		return dynamic_cast<T*>(raw_ptr);
	}
};
