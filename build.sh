#!/usr/bin/env sh
#Add debug flag when using static analyzer
#When running, you can do `./build.sh [any other flags you want to pass to compiler]`
clang++ -std=c++17 -Wall -Wextra -pedantic-errors $@ -lncurses -o editorial main.cpp syntax-highlight.cpp screen.cpp undo.cpp
