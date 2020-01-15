#ifndef SYNTAX_HIGHLIGHT_H
#define SYNTAX_HIGHLIGHT_H
#include "screen.h"

void markdown_mode(int start, int end);

void cpp_mode(int start_row, int end_row);
constexpr Color KeywordColor = Color::Cyan;
constexpr Color TypeColor = Color::Yellow;
constexpr Color PreprocessorColor = Color::Blue;
constexpr Color StringColor = Color::Green;
#endif
