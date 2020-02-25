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

/**Highlights some features of markdown files, including '*', '#', and
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
            case ' ':
                // Do nothing; no need to highlight spaces
                break;
	    default:
		if(in_inline_code) {
		    set_cell_color(col, row, InlineCodeColor);
		}
	    }
	}
    }
}

/**Highlights some of the common keywords and types of C++, as well as
   the text within string/character literals. start_row is the index of
   the row in the buffer at the top of the screen*/
void cpp_mode(int, int end_row)
{
    const int width = screen_width();
    bool in_string = false;
    bool in_comment = false;

    for(int row = 0; row < end_row; ++row) {
	int col = 0;
	while(col < width) {
	    auto character = screen_get(col, row);
	    // Handle string/comment highlighting, skip to next iteration
	    if(character == '"') {
                // String opening/closing
		in_string = !in_string;
		set_cell_color(col++, row, StringColor);
		continue;
	    } else if(in_string) {
                // Middle of string
		set_cell_color(col++, row, StringColor);
		continue;
	    } else if(character == '/' && screen_get(col + 1, row) == '*') {
                // Opening of multi-line comment
                in_comment = true;
                set_cell_color(col++, row, StringColor);
                set_cell_color(col++, row, StringColor);
                continue;
            } else if(character == '*' && screen_get(col + 1, row) == '/') {
                // Closing of multi-line comment
                in_comment = false;
                set_cell_color(col++, row, StringColor);
                set_cell_color(col++, row, StringColor);
                continue;
            } else if(in_comment) {
                // Middle of multi-line comment
                set_cell_color(col++, row, StringColor);
		continue;
            }

	    // Handle all other highlighting
	    switch(character) {
	    case '#':
		HIGHLIGHT_MATCH("#include", PreprocessorColor);
		HIGHLIGHT_MATCH("#define", PreprocessorColor);
                HIGHLIGHT_MATCH("#ifndef", PreprocessorColor);
                HIGHLIGHT_MATCH("#ifdef", PreprocessorColor);
                HIGHLIGHT_MATCH("#endif", PreprocessorColor);
		break;
            case '+': case '-': case '*': case '/':
            case '=': case '!': case '<': case '>':
            case '&': case '|': case '^': case '~':
                set_cell_color(col, row, KeywordColor);
                break;
	    case '\'':
                // Highlight entire character literal
		set_cell_color(col++, row, StringColor);
		set_cell_color(col++, row, StringColor);
		set_cell_color(col, row, StringColor);
		break;
            case 'a':
                HIGHLIGHT_MATCH("auto", TypeColor);
                break;
	    case 'b':
		HIGHLIGHT_MATCH("bool", TypeColor);
		HIGHLIGHT_MATCH("break", KeywordColor);
		break;
	    case 'c':
		HIGHLIGHT_MATCH("char", TypeColor);
		HIGHLIGHT_MATCH("case", KeywordColor);
		HIGHLIGHT_MATCH("constexpr", TypeColor);
		HIGHLIGHT_MATCH("const", TypeColor);
                HIGHLIGHT_MATCH("continue", KeywordColor);
		HIGHLIGHT_MATCH("class", KeywordColor);
                HIGHLIGHT_MATCH("catch", KeywordColor);
		break;
	    case 'd':
		HIGHLIGHT_MATCH("default", KeywordColor);
		HIGHLIGHT_MATCH("delete", KeywordColor);
                HIGHLIGHT_MATCH("do", KeywordColor);
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
                HIGHLIGHT_MATCH("try", KeywordColor);
		break;
            case 'u':
                HIGHLIGHT_MATCH("unsigned", TypeColor);
                break;
            case 'w':
                HIGHLIGHT_MATCH("while", KeywordColor);
                break;
	    }

	    ++col;
	}
    }
}
