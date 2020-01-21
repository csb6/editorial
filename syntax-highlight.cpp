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

    for(int row = start_row; row < end_row; ++row) {
	for(int col = 0; col < width; ++col) {
	    auto character = screen_get(col, row);
	    switch(character) {
	    case '#':
		if(match(col, row, "#include")) {
		    highlight(col, row, 8, PreprocessorColor);
		}
		break;
	    case '"':
		set_cell_color(col, row, StringColor);
		in_string = !in_string;
		continue;
	    case 'b':
		if(match(col, row, "bool")) {
		    highlight(col, row, 4, TypeColor);
		}
		break;
	    case 'c':
		if(match(col, row, "char")) {
		    highlight(col, row, 4, TypeColor);
		}
		break;
	    case 'e':
		if(match(col, row, "else")) {
		    highlight(col, row, 4, KeywordColor);
		}
		break;
	    case 'f':
		if(match(col, row, "for")) {
		    highlight(col, row, 3, KeywordColor);
		}
		break;
	    case 'i':
		if(match(col, row, "int")) {
		    highlight(col, row, 3, TypeColor);
		} else if(match(col, row, "if")) {
		    highlight(col, row, 2, KeywordColor);
		}
		break;
	    case 'r':
		if(match(col, row, "return")) {
		    highlight(col, row, 6, KeywordColor);
		}
		break;
	    }

	    if(in_string) {
		set_cell_color(col, row, StringColor);
	    }
	}
    }
}
