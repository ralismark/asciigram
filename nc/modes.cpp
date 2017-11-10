#include "modes.hpp"

Mode mode = Mode::Normal;

// wrapper for when we need to actually switch around layers
void setmode(Mode m)
{
	mode = m;
	ls.layers[1].reset();

	switch(mode) {
	case Mode::Normal:
		ls.layers[1] = std::make_unique<NormalMode>();
		break;
	case Mode::Box:
		ls.layers[1] = std::make_unique<BoxMode>();
		break;
	case Mode::Insert:
		ls.layers[1] = std::make_unique<InsertMode>();
		break;
	case Mode::Move:
		ls.layers[1] = std::make_unique<MoveMode>();
		break;
	case Mode::Quit:
		break;
	default:
		setmode(Mode::Normal);
	}
}
