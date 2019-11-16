#include <iostream>
#include "termbox.h"

static inline void present(int insert_x, int insert_y);

class TermboxApp {
public:
    int screen_width, screen_height;
    int insert_x = 0;
    int insert_y = 0;
    TermboxApp()
    {
	auto status = tb_init();
	if(status < 0) {
	    std::cerr << "Error: Termbox failed with code " << status << '\n';
	    tb_shutdown();
	    exit(1);
	}
	screen_width = tb_width();
	screen_height = tb_height();
    }
    void resize()
    {
	screen_width = tb_width();
	screen_height = tb_height();
	present(screen_width, screen_height);
    }
    ~TermboxApp() { tb_shutdown(); }
};

class Cell {
    
};

static inline void present(int insert_x, int insert_y)
{
    tb_set_cursor(insert_x, insert_y);
    tb_present();
}

int main()
{
    TermboxApp tb;
    tb_event event{};
    bool running = true;
    present(tb.insert_x, tb.insert_y);
    
    while(running && tb_peek_event(&event, 1) != -1) {
	if(event.type != TB_EVENT_KEY) {
	    continue;
	} else if(event.type == TB_EVENT_RESIZE) {
	    tb.resize();
	    continue;
	} else if(event.key == TB_KEY_CTRL_C) {
	    running = false;
	} else if(event.ch >= '!' || event.key == TB_KEY_SPACE) {
	    tb_change_cell(tb.insert_x++, tb.insert_y, event.ch, TB_WHITE, TB_BLACK);
	    if(tb.insert_x >= tb.screen_width) {
		++tb.insert_y;
		tb.insert_x = 0;
	    }
	    present(tb.insert_x, tb.insert_y);
	} else if(event.key == TB_KEY_BACKSPACE2) {
	    if(tb.insert_y <= 0 && tb.insert_x <= 0) continue;
	    if(tb.insert_x == 0) {
		tb.insert_x = tb.screen_width - 1;
		tb_change_cell(tb.insert_x, --tb.insert_y, ' ', TB_WHITE, TB_BLACK);
	    } else {
		tb_change_cell(--tb.insert_x, tb.insert_y, ' ', TB_WHITE, TB_BLACK);
	    }
	    present(tb.insert_x, tb.insert_y);
	}
    }

    return 0;
}
