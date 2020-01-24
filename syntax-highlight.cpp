#include <string_view>
#include "syntax-highlight.h"

/**Checks if the text on the screen, going left to right starting at a
   given coordinate, matches the content of the given target text*/
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

/**Highlights length number of characters onscreen with the given
   foreground color; bounds checks to stay within current screen size*/
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

/**If the given text matches the onscreen text starting at a given point,
   the matching text is highlighted onscreen*/
static bool highlight_match(int col, int row, std::string_view target, Color fg)
{
    if(match(col, row, target)) {
	highlight(col, row, target.size(), fg);
	return true;
    }
    return false;
}

/**Default highlighting mode; highlights nothing*/
void text_mode(int, int) {}

/**Highlights some features markdown files, including '*', '#', and
   inline code */
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

/**Highlights some of the common keywords and types of C++, as well as
   highlights the entire text within string/character literals*/
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
		if(!in_string) {
		    set_cell_color(col, row, StringColor);
		    in_char = !in_char;
		}
		break;
	    case '"':
		if(!in_char) {
		    set_cell_color(col, row, StringColor);
		    in_string = !in_string;
		}
		break;
	    case 'b':
		if(highlight_match(col, row, "bool", TypeColor)) {
		    col += 4;
		    continue;
		} else if(highlight_match(col, row, "break", KeywordColor)) {
		    col += 5;
		    continue;
		}
		break;
	    case 'c':
		if(highlight_match(col, row, "char", TypeColor)) {
		    col += 4;
		    continue;
		} else if(highlight_match(col, row, "case", KeywordColor)) {
		    col += 4;
		    continue;
		}
		break;
	    case 'd':
		if(highlight_match(col, row, "default", KeywordColor)) {
		    col += 7;
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
	    case 's':
		if(highlight_match(col, row, "switch", KeywordColor)) {
		    col += 5;
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
