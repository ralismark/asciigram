#pragma once

#include <vector>
#include <memory>

/**
 * Interface for an input and output layer. This allows for a modular UI
 * design, with new functionality added easily. Each layer should provide a
 * specific service (e.g. changing the style). These are similar to modes in
 * Emacs or Vim, and can be used for a similar purpose.
 *
 * The implementer must implement event(), which handles NCurses input events
 * (from getch). Optionally, they can also implement frame() for events done
 * before rendering, or post(), for things done after.
 */
struct Layer
{
	using event_type = int;
public:
	virtual ~Layer() = default;

	/**
	 * Handle input events, optionally propagating them to lower layers.
	 * The implementer must return true to propagate events (e.g. if they
	 * are not handled), false to stop.
	 */
	virtual bool event(event_type) = 0;

	/**
	 * (Optionally) Perform actions after events are handled but before
	 * rendering. This is always executed from the bottom up.
	 *
	 * This may be useful to respond to cursor movement (which is usually
	 * handled by the Universal layer) without manually reacting to all
	 * movement keys.
	 */
	virtual void frame() {}

	/**
	 * (Optionally) Perform actions after elements rendering, such as to
	 * show pop-up dialog boxes.
	 *
	 * Note that if drawing to stdscr, wnoutrefresh must be called on the
	 * stdscr to apply the changes, since it is normally done before post()
	 * (to correctly order pop-ups).
	 */
	virtual void post() {}
};

/**
 * An ordered collection of layers, handling event propagation and frame/post
 * actions.
 */
struct LayerStack
{
	std::vector<std::unique_ptr<Layer>> layers;
public:
	/**
	 * Propagate an event from the top of the layer stack, stopping when
	 * the bottom is reached or a layer stops propagation.
	 *
	 * Returns if the event passed through all layers without being
	 * stopped (i.e. all layers returned true).
	 */
	bool event(Layer::event_type val)
	{
		for(auto it = layers.rbegin(); it != layers.rend(); ++it) {
			if(*it) {
				bool propagate = (*it)->event(val);
				if(!propagate) {
					return false;
				}
			}
		}
		return true;
	}

	/**
	 * Call Layout::frame() for all layers, doing so from the bottom to the
	 * top.
	 */
	void frame()
	{
		for(auto& layer : layers) {
			if(layer) {
				layer->frame();
			}
		}
	}

	/**
	 * Call Layout::post() for all layers, doing so from the bottom to the
	 * top.
	 */
	void post()
	{
		for(auto& layer : layers) {
			if(layer) {
				layer->post();
			}
		}
	}
};
