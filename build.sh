#!/usr/bin/env sh

#Set this to your compiler (e.g. g++)
compiler=clang++

#Add debug flag when using static analyzer
#When running, you can do `./build.sh [any other flags you want to pass to compiler]`
$compiler -std=c++17 -Wall -Wextra -pedantic-errors $@ -lncurses -o build/editorial main.cpp syntax-highlight.cpp screen.cpp undo.cpp build/cpp_matcher.o build/mips_matcher.o
