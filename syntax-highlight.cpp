#include <string_view>
#include <string> // for std::char_traits
#include "syntax-highlight.h"
#include "cpp_matcher.h"
#include "mips_matcher.h"

/**Highlights length number of characters onscreen with the given
   foreground color; bounds checks to stay within current screen size*/
static void highlight(Screen &window, int col, int row, int length, Color fg)
{
    const int width = window.width();
    const int height = window.height();
    int i = 0;
    for(; row < height; ++row) {
	for(; col < width; ++col) {
	    if(i >= length) return;
	    window.set_color(col, row, fg);
	    ++i;
	}
    }
}

/**Default highlighting mode; highlights nothing*/
void text_mode(Screen&, int, int) {}

/**Highlights some features of markdown files, including '*', '#', and
   inline code */
void markdown_mode(Screen &window, int, int end_row)
{
    bool in_inline_code = false;
    const int width = window.width();
    for(int row = 0; row < end_row; ++row) {
	for(int col = 0; col < width; ++col) {
	    auto character = window.get(col, row);
	    switch(character) {
	    case '*':
	        window.set_color(col, row, ItalicColor);
		break;
	    case '#':
	        window.set_color(col, row, TitleColor);
		break;
	    case '`':
	        window.set_color(col, row, InlineCodeColor);
		in_inline_code = !in_inline_code;
		break;
            case ' ':
                // Do nothing; no need to highlight spaces
                break;
	    default:
		if(in_inline_code) {
		    window.set_color(col, row, InlineCodeColor);
		}
	    }
	}
    }
}

/**Highlights some of the common keywords and types of C++, as well as
   the text within string/character literals. start_row is the index of
   the row in the buffer at the top of the screen*/
void cpp_mode(Screen &window, int, int end_row)
{
    const int width = window.width();
    bool in_string = false;
    bool in_comment = false;

    for(int row = 0; row < end_row; ++row) {
	int col = 0;
	while(col < width) {
	    auto character = window.get(col, row);
	    // Handle string/comment highlighting, skip to next iteration
	    if(character == '"') {
                // String opening/closing
		in_string = !in_string;
		window.set_color(col++, row, StringColor);
		continue;
	    } else if(in_string) {
                // Middle of string
		window.set_color(col++, row, StringColor);
		continue;
	    } else if(character == '/' && window.get(col + 1, row) == '*') {
                // Opening of multi-line comment
                in_comment = true;
                window.set_color(col++, row, StringColor);
                window.set_color(col++, row, StringColor);
                continue;
            } else if(character == '*' && window.get(col + 1, row) == '/') {
                // Closing of multi-line comment
                in_comment = false;
                window.set_color(col++, row, StringColor);
                window.set_color(col++, row, StringColor);
                continue;
            } else if(in_comment) {
                // Middle of multi-line comment
                window.set_color(col++, row, StringColor);
		continue;
            }

            // TODO: In code gen, fix issue with const not being highlighted
            auto[is_match, color, len] = match_cpp(window, col, row);
            if(is_match) {
                highlight(window, col, row, len, color);
                col += len;
            } else {
                ++col;
            }
        }
    }
}

/**Highlights most instructions/registers of the MIPS-32 assembly language*/
void mips_mode(Screen &window, int, int end_row)
{
    const int width = window.width();

    for(int row = 0; row < end_row; ++row) {
	int col = 0;
	while(col < width) {
            auto[is_match, color, len] = match_mips(window, col, row);
            if(is_match) {
                highlight(window, col, row, len, color);
                col += len;
            } else {
                ++col;
            }
        }
    }
}
