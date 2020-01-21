#!/usr/bin/env sh
#Add debug flag when using static analyzer
clang++ -std=c++17 -Wall -Wextra -pedantic-errors -lncurses -o editor-min main.cpp syntax-highlight.cpp screen.cpp
