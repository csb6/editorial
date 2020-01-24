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
- Works on any terminal supported by ncurses (essentially anything)

## Key Bindings

**Ctrl-s** : Save the file to disk

**Ctrl-c** : Quit (without saving)

**Left and Right Arrows** : Move cursor one character forward/backward

**Up and Down Arrows** : Move cursor one line up/down

**Tab** : Insert four spaces at the cursor

## Installation

Just run `./build.sh`. Make sure that you have ncurses installed and
in a location that your compiler can find. Most Unix-like systems should
have an ncurses shared library installed, but on Windows you will most
likely have to install PDCurses, which implements the same interface as
ncurses.