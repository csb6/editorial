# Editorial

A text editor for your terminal, written in C++17.

## Key Bindings

**Ctrl-s** : Save the file to disk

**Ctrl-c** : Quit (without saving)

**Left and Right Arrows** : Move cursor one character forward/backward

**Up and Down Arrows** : Move cursor one line up/down

**Tab** : Insert four spaces at the cursor

## Installation

The editor should run in any terminal that supports terminfo.

After cloning the repository and its submodule (termbox), run `build-full.sh`.
This will build `termbox`, the C library used to draw to the terminal screen.

Only call `build-full.sh` after cloning or updating the submodule; otherwise,
to rebuild the program after changes to the program's source code, run
`build.sh`, which will not rebuild the termbox library. The termbox binary,
(named `libtermbox.a` or similar), will always be statically linked.