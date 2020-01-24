#include <string_view>
#include "syntax-highlight.h"

static bool match(int col, int row, std::string_view target)
{
    const int width = screen_width();
    for(std::size_t i = 0; i < target.size(); ++i) {
	if(col >= width || screen_get(col, row) != target[i])
	    return false;
	++col;
    }
    return true;
}

static void highlight(int col, int row, int length, Color fg)
{
    const int width = screen_width();
    const int height = screen_height();
    int i = 0;
    for(; row < height; ++row) {
	for(; col < width; ++col) {
	    if(i >= length) return;
	    set_cell_color(col, row, fg);
	    ++i;
	}
    }
}

static bool highlight_match(int col, int row, std::string_view target, Color fg)
{
    if(match(col, row, target)) {
	highlight(col, row, target.size(), fg);
	return true;
    }
    return false;
}

void text_mode(int, int) {}

void markdown_mode(int start_row, int end_row)
{
    bool in_inline_code = false;
    const int width = screen_width();
    for(int row = start_row; row < end_row; ++row) {
	for(int col = 0; col < width; ++col) {
	    auto character = screen_get(col, row);
	    switch(character) {
	    case '*':
	        set_cell_color(col, row, ItalicColor);
		break;
	    case '#':
	        set_cell_color(col, row, TitleColor);
		break;
	    case '`':
	        set_cell_color(col, row, InlineCodeColor);
		in_inline_code = !in_inline_code;
		break;
	    default:
		if(character != ' ' && in_inline_code) {
		    set_cell_color(col, row, InlineCodeColor);
		}
	    }
	}
    }
}

void cpp_mode(int start_row, int end_row)
{
    const int width = screen_width();
    bool in_string = false;
    bool in_char = false;

    for(int row = start_row; row < end_row; ++row) {
	int col = 0;
	while(col < width) {
	    auto character = screen_get(col, row);
	    switch(character) {
	    case '#':
		if(highlight_match(col, row, "#include", PreprocessorColor)) {
		    col += 8;
		    continue;
		}
		break;
	    case '\'':
		set_cell_color(col, row, StringColor);
		in_char = !in_char;
		break;
	    case '"':
		set_cell_color(col, row, StringColor);
		in_string = !in_string;
		break;
	    case 'b':
		if(highlight_match(col, row, "bool", TypeColor)) {
		    col += 4;
		    continue;
		}
		break;
	    case 'c':
		if(highlight_match(col, row, "char", TypeColor)) {
		    col += 4;
		    continue;
		}
		break;
	    case 'e':
		if(highlight_match(col, row, "else", KeywordColor)) {
		    col += 4;
		    continue;
		}
		break;
	    case 'f':
		if(highlight_match(col, row, "for", KeywordColor)) {
		    col += 3;
		    continue;
		}
		break;
	    case 'i':
		if(highlight_match(col, row, "int", TypeColor)) {
		    col += 3;
		    continue;
		} else if(highlight_match(col, row, "if", KeywordColor)) {
		    col += 2;
		    continue;
		}
		break;
	    case 'r':
		if(highlight_match(col, row, "return", KeywordColor)) {
		    col += 6;
		    continue;
		}
		break;
	    }

	    if(in_string || in_char) {
		set_cell_color(col, row, StringColor);
	    }
	    ++col;
	}
    }
}
