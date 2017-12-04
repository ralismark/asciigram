# Asciigram (work in progress)

Drawing ASCII diagrams by hand is hard. This should simplify that.

Currently in development, nothing is final and many key features are missing.

## Building

NCurses (or similar) is required. Apart from that, everything is standard C++14,
and should be compatible with all platforms.

CMake is used to build:

	$ mkdir build && cd build
	$ cmake ..
	$ cmake --build .

## Getting Started

After building, the NCurses frontend is available as `nc`.

	$ ./nc

It is recommended that you read the help, which is available by pressing `?`.
You can quit by pressing `q` several times.
