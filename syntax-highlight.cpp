#include <string_view>
#include "termbox.h"
#include "syntax-highlight.h"

static bool match(const tb_cell *screen, int start, std::string_view target)
{
    if(start + target.size() > static_cast<unsigned long>(tb_width() * tb_height()))
	return false;

    for(std::size_t i = 0; i < target.size(); ++i) {
	if(screen[start+i].ch != static_cast<uint32_t>(target[i]))
	    return false;
    }
    return true;
}

constexpr static void highlight(tb_cell *screen, int start, int end, uint32_t fg)
{
    for(; start < end; ++start) {
	screen[start].fg = fg;
    }
}

void markdown_mode(int start_row, int end_row)
{
    tb_cell *screen = tb_cell_buffer();
    bool in_inline_code = false;

    const int end = end_row * tb_width();
    for(int i = start_row * tb_width(); i < end; ++i) {
	switch(screen[i].ch) {
	case '*':
	    screen[i].fg = TB_YELLOW;
	    break;
	case '#':
	    screen[i].fg = TB_BLUE;
	    break;
	case '`':
	    screen[i].fg = TB_GREEN;
	    in_inline_code = !in_inline_code;
	    break;
	default:
	    if(screen[i].ch != ' ' && in_inline_code) {
		screen[i].fg = TB_GREEN;
	    }
	}
    }
}

void cpp_mode(int start_row, int end_row)
{
    tb_cell *screen = tb_cell_buffer();
    const int end = end_row * tb_width();
    bool in_string = false;

    for(int i = start_row * tb_width(); i < end; ++i) {
	switch(screen[i].ch) {
	case '#':
	    if(match(screen, i, "#include")) {
		highlight(screen, i, i+8, PreprocessorColor);
	    }
	    break;
	case '"':
	    screen[i].fg = StringColor;
	    in_string = !in_string;
	    continue;
	case 'b':
	    if(match(screen, i, "bool")) {
		highlight(screen, i, i+4, TypeColor);
	    }
	    break;
	case 'c':
	    if(match(screen, i, "char")) {
		highlight(screen, i, i+4, TypeColor);
	    }
	    break;
	case 'e':
	    if(match(screen, i, "else")) {
		highlight(screen, i, i+4, KeywordColor);
	    }
	    break;
	case 'f':
	    if(match(screen, i, "for")) {
		highlight(screen, i, i+3, KeywordColor);
	    }
	    break;
	case 'i':
	    if(match(screen, i, "int")) {
		highlight(screen, i, i+3, TypeColor);
	    } else if(match(screen, i, "if")) {
		highlight(screen, i, i+2, KeywordColor);
	    }
	    break;
	case 'r':
	    if(match(screen, i, "return")) {
		highlight(screen, i, i+6, KeywordColor);
	    }
	    break;
	}

	if(in_string) {
	    screen[i].fg = StringColor;
	}
    }
}
