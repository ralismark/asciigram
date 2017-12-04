#pragma once

/**
 * \file
 * Define globals used throughout the program to refer to its state. Passing
 * state directly would be too messy, and these variables would have to be
 * always passed anyways.
 */

#include "layer.hpp"

#include "../drawable.hpp"
#include "../multistyle.hpp"

/**
 * Manager for all styles across the program. This holds the available styles,
 * as well as provides them to new elements.
 */
extern MultiStyleManager msm;

/**
 * The main stack of all elements. This may contain lower ElementStacks
 * (groups), but those may not contain any ElementStack.
 */
extern ElementStack es;

/**
 * Layer of UI interactions. The first layer is the Universal layer, then the
 * mode, and after than any popup dialogs.
 */
extern LayerStack ls;
