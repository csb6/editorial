#ifndef SYNTAX_HIGHLIGHT_H
#define SYNTAX_HIGHLIGHT_H
#include "screen.h"

void text_mode(Screen&, int, int);

void markdown_mode(Screen &window, int start, int end);
constexpr Color ItalicColor = Color::Yellow;
constexpr Color TitleColor = Color::Blue;
constexpr Color InlineCodeColor = Color::Green;

void cpp_mode(Screen &window, int start_row, int end_row);
constexpr Color KeywordColor = Color::Cyan;
constexpr Color TypeColor = Color::Yellow;
constexpr Color PreprocessorColor = Color::Magenta;
constexpr Color StringColor = Color::Green;

void mips_mode(Screen &window, int, int end_row);
constexpr Color RegColor = Color::Green;
constexpr Color InstructColor = Color::Cyan;
#endif
