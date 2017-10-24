#pragma once

#include "drawable.hpp"

#include <vector>
#include <memory>
#include <utility>

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
