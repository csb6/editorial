#include <string_view>
#include <string> // for std::char_traits
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
   highlight the matching text onscreen*/
#define HIGHLIGHT_MATCH(T, C) \
if(match(col, row, T)) { \
    highlight(col, row, std::char_traits<char>::length(T), C); \
    col += std::char_traits<char>::length(T); \
    continue; \
}

/**Default highlighting mode; highlights nothing*/
void text_mode(int, int) {}

/**Highlights some features markdown files, including '*', '#', and
   inline code */
void markdown_mode(int, int end_row)
{
    bool in_inline_code = false;
    const int width = screen_width();
    for(int row = 0; row < end_row; ++row) {
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
   the text within string/character literals. start_row is the index of
   the row in the buffer at the top of the screen*/
void cpp_mode(int start_row, int end_row)
{
    const int width = screen_width();
    bool in_string = false;
    // The value of start_row in the prior call
    static int last_start_row = 0;
    // The value of in_string at the end of the prior call
    static bool last_in_string = false;
    if(last_start_row < start_row)
        // Only worry about any open strings if we've scrolled down
        in_string = last_in_string;

    for(int row = 0; row < end_row; ++row) {
	int col = 0;
	while(col < width) {
	    auto character = screen_get(col, row);
	    // Handle string highlighting
	    if(character == '"') {
		in_string = !in_string;
		set_cell_color(col++, row, StringColor);
		continue;
	    } else if(in_string) {
		set_cell_color(col++, row, StringColor);
		continue;
	    }
	    // Handle all other highlighting
	    switch(character) {
	    case '#':
		HIGHLIGHT_MATCH("#include", PreprocessorColor);
		HIGHLIGHT_MATCH("#define", PreprocessorColor);
		break;
	    case '\'':
		set_cell_color(col++, row, StringColor);
		set_cell_color(col++, row, StringColor);
		set_cell_color(col++, row, StringColor);
		break;
	    case 'b':
		HIGHLIGHT_MATCH("bool", TypeColor);
		HIGHLIGHT_MATCH("break", KeywordColor);
		break;
	    case 'c':
		HIGHLIGHT_MATCH("char", TypeColor);
		HIGHLIGHT_MATCH("case", KeywordColor);
		HIGHLIGHT_MATCH("const", TypeColor);
		HIGHLIGHT_MATCH("class", KeywordColor);
		break;
	    case 'd':
		HIGHLIGHT_MATCH("default", KeywordColor);
		HIGHLIGHT_MATCH("delete", KeywordColor);
		break;
	    case 'e':
		HIGHLIGHT_MATCH("else", KeywordColor);
		HIGHLIGHT_MATCH("enum", KeywordColor);
		break;
	    case 'f':
		HIGHLIGHT_MATCH("for", KeywordColor);
		HIGHLIGHT_MATCH("false", KeywordColor);
		break;
	    case 'i':
		HIGHLIGHT_MATCH("int", TypeColor);
		HIGHLIGHT_MATCH("if", KeywordColor);
		break;
	    case 'n':
		HIGHLIGHT_MATCH("new", KeywordColor);
		break;
	    case 'p':
		HIGHLIGHT_MATCH("public:", KeywordColor);
		HIGHLIGHT_MATCH("private:", KeywordColor);
		break;
	    case 'r':
		HIGHLIGHT_MATCH("return", KeywordColor);
		break;
	    case 's':
		HIGHLIGHT_MATCH("switch", KeywordColor);
		break;
	    case 't':
		HIGHLIGHT_MATCH("true", KeywordColor);
		break;
	    }

	    ++col;
	}
    }
    
    last_start_row = start_row;
    last_in_string = in_string;
}
