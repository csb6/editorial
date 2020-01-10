#include <string_view>
#include "termbox.h"

static bool matches(const tb_cell *screen, std::size_t size, std::size_t pos,
		    std::string_view target)
{
    if(pos + target.size() > size)
	return false;
    for(std::size_t i = 0; i < target.size(); ++i) {
	if(screen[pos+i].ch != (uint32_t)target[i])
	    return false;
    }
    return true;
}

static void set_fg(tb_cell *screen, int start, int end,
		   uint16_t color = TB_DEFAULT)
{
    for(int i = start; i < end; ++i) {
	screen[i].fg = color;
    }
}

void highlight(int start)
{
    tb_cell *screen = tb_cell_buffer();
    const int size = tb_width() * tb_height();
    for(int i = start; i < size; ++i) {
	if(screen[i].ch == ' ')
	    continue;
	else if(screen[i].ch == '*' && matches(screen, size, i, "**")) {
	    set_fg(screen, i, i+2, TB_YELLOW);
	} else if(screen[i].ch == '#' && matches(screen, size, i, "#")) {
	    screen[i].fg = TB_YELLOW;
	}
    }
}
