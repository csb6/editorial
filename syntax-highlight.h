#ifndef SYNTAX_HIGHLIGHT_H
#define SYNTAX_HIGHLIGHT_H
#include "termbox.h"

void markdown_mode(int start, int end);

void cpp_mode(int start_row, int end_row);
constexpr uint32_t KeywordColor = TB_CYAN;
constexpr uint32_t TypeColor = TB_YELLOW;
constexpr uint32_t PreprocessorColor = TB_BLUE;
constexpr uint32_t StringColor = TB_GREEN;
#endif
