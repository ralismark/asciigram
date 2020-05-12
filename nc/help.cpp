#include "help.hpp"

/**
 * The actual help message. Since global variable are somewhat messy, a
 * function returning a constant provides similar functionality without any
 * negatives.
 */
static const char* helpmsg()
{
	return R"(
Asciigram is a tool to help you make plain text diagrams, for when you cannot
insert images or draw. Currently, Asciigram is currently in early development,
though some aspects are already quite usable.

Key bindings and interactions are done in different modes (like vim), such as:
 - Normal mode
 - Box mode (for making boxes)
 - Move mode (for moving elements)
 - Insert mode (for adding raw text)
 - Arrow mode (for drawing arrows)

However, a layered binding system exists, in which additional functionality
(e.g. menus) are added over existing modes/layers without affecting them. For
example, a 'Universal' layer exists below all mode layers, which contains
bindings usable almost everywhere.

========== Universal ===========================================================
  These key bindings are available everywhere, unless another mode/layer uses
  them.

        ESC     Leave the existing mode and go to normal mode. Usually also
                  exits sub-layers (e.g. style menu), or finishes elements
        ?       Show the help layer (what you're seeing here)
        h       Move cursor left
        j       Move cursor down
        k       Move cursor up
        l       Move cursor right
        H       Scroll screen to the left (moving everything right)
        J       Scroll screen down (moving everything up)
        K       Scroll screen up (moving everything down)
        L       Scroll screen to the right (moving everything left)
        q       Quit (even through other modes)

========== Normal Mode =========================================================
  Normal mode is the default mode, when you're not doing anything too special.
  You are able to perform basic operations on elements. From this mode, you can
  also go into modes for inserting various elements.

        x       Remove the top element under the cursor (into the register)
        p       Paste the item in the register, keeping the offset of the cursor
        v       Enter visual (block) mode
        b       Enter box mode
        i       Enter insert mode
        a       Enter arrow mode
        m       Enter move mode (with element under cursor)
        s       Open style pop-up for boxes
        S       Open style pop-up for arrows
        <       Lower item below cursor by one level
        >       Raise item below cursor by one level

========== Move Mode ===========================================================
  This mode allows you to move elements around (without modifying them). The
  element moved is the one under the cursor when this mode was started. If
  nothing is under the cursor, this returns to normal mode.

        m       Exit move mode, returning to normal mode
        h       Move element left
        j       Move element down
        k       Move element up
        l       Move element right

========== Visual Mode =========================================================
  Visual mode allows you to select elements in a rectangular region and perform
  operations on multiple elements. Notably, you can group them, allowing you
  to act as if several elements are one. Every element that the selection
  touches is considered part of the selection.

        v       Exit visual mode and return to normal mode
        o       Go to Other end of box, as if the current cursor position was
                  the start of the box, and continue dragging out the box from
                  the original start
        O       Go to the Opposite side of the same line, changing from right
                  corner to left (and vice versa). Similar to (lower-case) o
        g       Combine selected elements into a single group
        x       Delete selected elements
        c       Copy selected region into the system clipboard
        y       Copy selected elements into register as a group

========== Box Mode ============================================================
  Rectangular boxes can be drawn in this mode. The box can be dragged out by
  moving the cursor (e.g. hjkl). The style of a box can be changed using the
  style pop-up ('s' in normal mode).

  For vim users, the controls are similar to those block-wise visual mode.

        x       Delete current box, returning to normal mode
        b       Finish the box, returning to normal mode
        o       Go to Other end of box, as if the current cursor position was
                  the start of the box, and continue dragging out the box from
                  the original start
        O       Go to the Opposite side of the same line, changing from right
                  corner to left (and vice versa). Similar to (lower-case) o

When the cursor is moved, the active corner of the box is moved to it,
expanding or contracting it.

========== Insert Mode =========================================================
  Plain text can be inserted while in Insert mode. While in this mode, anything
  you type will be inserted as text. You can exit with escape.

        ESC     Finish text block, returning to normal mode

========== Arrow Mode ==========================================================
  Arrow mode allows you to draw arrows. Arrows are represented as a sequence of
  checkpoints/marks (which the arrow must pass through), and the orientation
  between them. This allows you to make arrows of any kind.

        x       Delete the current arrow and return to normal mode
        a       Create a new checkpoint. Doing this twice at a spot finishes
                  the arrow
        o       Change orientation of the current segment

========== Style Pop-up ========================================================

                           .- Change style .
                           |       |       |
                           |  123  |  .-.  |
                    e.g.   |  456  |  | |  |
                           |  789  |  '-'  |
                           |       |       |
                           '---------------'

  The style pop-up can be used to change the style used for new and existing
  elements. The numbers/letters on the left are used to select what the part of
  the object on the right to change style for. After pressing the letter, the
  new character for that part can be pressed to set it. Escape cancels it, and
  backspace makes it transparent.

  After pressing the placeholder, the corresponding component is temporarily set to '#'
  both in the style dialog and for all elements.

        q       Close the style pop-up
        +       Duplicate the current style
        [       Switch to the previous style
        ]       Switch to the next style

  The style which is used for new elements is the currently shown one in the
  dialog, which can be changed with [ and ].

========== Pending Features/TODO ===============================================
Asciigram is in development, so there are several features which need to be
implemented, some of which are integral to its use.
 - Completing the help pop-up
 - Exporting to ASCII
 - Saving
 - Arrow styling
 - Template elements
 - Container elements (e.g. text in a diamond)
)";

}

/**
 * Split a string into its lines.
 */
static std::vector<std::string> split_lines(const std::string& s)
{
	std::vector<std::string> lines;
	std::string line;
	std::istringstream iss{s};
	while (std::getline(iss, line))
	{
		lines.push_back(line);
	}
	return lines;
}


const std::vector<std::string>& helplines()
{
	static auto lines = split_lines(helpmsg());
	return lines;
}
