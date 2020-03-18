# Editorial

A text editor for your terminal, written in C++17.

**Please note:** This is probably not a replacement for your
editor of choice. It has a pretty small feature set, but I hope that
you find it interesting, at least from a programming standpoint.

## Features

- Fast syntax highlighting for C++, Markdown, and MIPS assembly
  (all activated by the file extension)
- Scrolling using arrow keys
- File saving/loading, with editing occurring in-memory
- Small implementation; around 800 lines of C++ code (not including generated code)
- Low CPU and memory usage
- Works on any terminal supported by ncurses (essentially anything)

## Key Bindings

**Ctrl-s** : Save the file to disk

**Ctrl-c** : Quit (without saving)

**Left and Right Arrows** : Move cursor one character forward/backward

**Up and Down Arrows** : Move cursor one line up/down

**Tab** : Insert four spaces at the cursor

## Installation

When you first clone the repository, run `./build-full.sh`. This
script will do the code-generation necessary to do fast syntax
highlighting for C++. This script will call both `build-code-gen.sh`
and `./build.sh`.

After the initial cloning, just run `./build.sh` to rebuild after any changes
to the main source code of the program (i.e. everything besides the C++
syntax highlighting). If you do modify any of the code generation code,
run `./build-full.sh` to get a build reflecting those changes.

The build scripts are set up to use `clang++` as the compiler, but you can
easily change this within each script. The scripts are all very short, so this
should be straightforward. Also, be sure that you have ncurses installed
and in a location that your compiler can find.

Most Unix-like systems should already have an ncurses shared library, but
on Windows you will most likely have to install PDCurses, which implements
the same interface as ncurses on Windows platforms. This editor only uses the
most basic features of ncurses (no fancy widgets or anything), so it should be
compatible with PDCurses. However, this has not been tested.

If there is no viable ncurses support for your platform, it should be
straightforward to port over the necessary terminal drawing code.
See `screen.h` and `screen.cpp` for the implementation of the ncurses wrapper.