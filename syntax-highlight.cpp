#include <string_view>
#include "termbox.h"

void highlight(int start_row, int end_row)
{
    tb_cell *screen = tb_cell_buffer();
    const int start = start_row * tb_width();
    const int end = end_row * tb_width();

    bool in_string = false;
    for(int i = start; i < end; ++i) {
	switch(screen[i].ch) {
	case '*':
	    screen[i].fg = TB_YELLOW;
	    break;
	case '#':
	    screen[i].fg = TB_BLUE;
	    break;
	case '"':
	    screen[i].fg = TB_GREEN;
	    in_string = !in_string;
	    break;
	}

	if(in_string) {
	    screen[i].fg = TB_GREEN;
	}
    }
}
