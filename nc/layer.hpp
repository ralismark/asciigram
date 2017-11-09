#pragma once

#include <vector>
#include <memory>

// An input and frame layer
struct Layer
{
	using event_type = int;
public:
	virtual ~Layer() = default;

	// allows layer to handle events, return false to stop propagation
	// events are propagated from top to bottom
	virtual bool event(event_type) = 0;

	// run stuff after events, per frame
	virtual void frame() {}

	// run stuff after rendering (e.g. for higher windows)
	virtual void post() {}
};

// a sequence of layers
struct LayerStack
{
	std::vector<std::unique_ptr<Layer>> layers;
public:
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

	void frame()
	{
		for(auto& layer : layers) {
			if(layer) {
				layer->frame();
			}
		}
	}

	void post()
	{
		for(auto& layer : layers) {
			if(layer) {
				layer->post();
			}
		}
	}
};
