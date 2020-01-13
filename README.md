# Editorial

A text editor for your terminal, written in C++17.

**Please note:** This is probably not a replacement for your
editor of choice. It has a pretty small feature set, but I hope that
you find it interesting, at least from a programming standpoint.

## Features

- Syntax highlighting, with simple way of adding new highlighting
  rules for new languages
- Basic file saving/loading
- Very simple implementation; under 500 lines of C++ code
- Low CPU and memory usage
- Works on any terminal supported by termbox
(essentially any commonly-used terminal for any commonly-used Unix-like
system).

## Key Bindings

**Ctrl-s** : Save the file to disk

**Ctrl-c** : Quit (without saving)

**Left and Right Arrows** : Move cursor one character forward/backward

**Up and Down Arrows** : Move cursor one line up/down

**Tab** : Insert four spaces at the cursor

## Installation

After cloning the repository and its submodule (termbox), run `build-full.sh`.
This will build `termbox`, the C library used to draw to the terminal screen.

Only call `build-full.sh` after cloning or updating the submodule; otherwise,
to rebuild the program after changes to this repo's source code, run
`build.sh`, which will build only the application code. The termbox binary,
(named `libtermbox.a` or similar), will always be statically linked.