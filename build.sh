#!/usr/bin/env sh
#Add debug flag when using static analyzer
clang++ -std=c++17 -Wall -Wextra -pedantic-errors -o editor-min main.cpp libtermbox.a
