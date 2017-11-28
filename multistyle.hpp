#pragma once

/**
 * \file
 * This file defines MultiStyleManager, which is a type to contain and manage
 * multiple StyleManagers (for different styles).
 */

#include "style.hpp"

#include <map>
#include <typeindex>
#include <typeinfo>

/**
 * Container for multiple style types.
 *
 * This provides a non-template container for the (templated) StyleManager,
 * automatically constructing a new StyleManager when needed.
 */
struct MultiStyleManager
{
	// To store StyleManagers
	struct TypeErased
	{
		virtual ~TypeErased() = default;
		virtual void* get() = 0;
		virtual const void* get() const = 0;
	};

	template <typename T>
	struct TypePreserved
		: public TypeErased
	{
		StyleManager<T> sm;
	public:
		virtual ~TypePreserved() = default;
		virtual void* get() { return &sm; }
		virtual const void* get() const { return &sm; }
	};

	// type erased StyleManagers
	std::map<std::type_index, std::unique_ptr<TypeErased>> stylemans;
public:
	/**
	 * Clear and construct (if needed) a StyleManager for type \p T.
	 */
	template <typename T>
	StyleManager<T>& reset()
	{
		auto store = std::make_unique<TypePreserved<T>>();
		auto* ref = static_cast<StyleManager<T>*>(store->get());
		stylemans.emplace(typeid(T), std::move(store));
		return *ref;
	}

	/**
	 * Access a StyleManager for a specific type \p T, constructing it if
	 * needed. This is similar to operator[] for std::map.
	 */
	template <typename T>
	StyleManager<T>& get()
	{
		if(stylemans.count(typeid(T)) > 0) {
			void* erased = stylemans.at(typeid(T))->get();
			return *static_cast<StyleManager<T>*>(erased);
		}
		return this->reset<T>();
	}
};
