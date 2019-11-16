#include <iostream>
#include "termbox.h"

void editor_present(int insert_x, int insert_y)
{
    tb_set_cursor(insert_x, insert_y);
    tb_present();
}

int main()
{
    auto status = tb_init();
    if(status < 0) {
	std::cerr << "Error: Termbox failed with code " << status << '\n';
	tb_shutdown();
	return 1;
    }
    int screen_width = tb_width();
    int screen_height = tb_height();
    tb_event event{};
    bool running = true;
    int insert_x = 0;
    int insert_y = 0;
    tb_set_cursor(insert_x, insert_y);
    tb_present();
    while(running && tb_peek_event(&event, 2) != -1) {
	if(event.type != TB_EVENT_KEY) {
	    continue;
	}
	if(event.key == TB_KEY_CTRL_C) {
	    running = false;
	} else if(event.ch >= '!' || event.key == TB_KEY_SPACE) {
	    tb_change_cell(insert_x++, insert_y, event.ch, TB_WHITE, TB_BLACK);
	    if(insert_x >= screen_width) {
		++insert_y;
		insert_x = 0;
	    }
	    tb_set_cursor(insert_x, insert_y);
	    tb_present();
	} else if(event.key == TB_KEY_BACKSPACE2) {
	    if(insert_y <= 0 && insert_x <= 0) continue;
	    if(insert_x == 0) {
		insert_x = screen_width - 1;
		tb_change_cell(insert_x, --insert_y, ' ', TB_WHITE, TB_BLACK);
	    } else {
		tb_change_cell(--insert_x, insert_y, ' ', TB_WHITE, TB_BLACK);
	    }
	    tb_set_cursor(insert_x, insert_y);
	    tb_present();
	}
    }

    tb_shutdown();
    return 0;
}
