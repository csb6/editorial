#include <iostream>
#include "termbox.h"
#include <list>

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
	    std::clog << "Error: Termbox failed with code " << status << std::endl;
	    tb_shutdown();
	    exit(1);
	}
	screen_width = tb_width();
	screen_height = tb_height();
    }
    void resize(const std::list<char> &buffer)
    {
	tb_clear();
	screen_width = tb_width();
	screen_height = tb_height();
	int row = 0;
	int col = 0;
	for(char each : buffer) {
	    tb_change_cell(col++, row, each, TB_WHITE, TB_BLACK);
	    if(col >= screen_width) {
		col = 0;
		++row;
	    }
	    if(row >= screen_height) {
		break;
	    }
	}
	insert_x = insert_y = 0;
	present(insert_x, insert_y);
    }
    ~TermboxApp() { tb_shutdown(); }
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
    std::list<char> buffer;
    present(tb.insert_x, tb.insert_y);
    
    while(running && tb_peek_event(&event, 1) != -1) {
	if(event.type == TB_EVENT_RESIZE) {
	    tb.resize(buffer);
	    continue;
	} else if(event.type != TB_EVENT_KEY) {
	    continue;
	} else if(event.key == TB_KEY_CTRL_C) {
	    running = false;
	} else if(event.ch >= '!' || event.key == TB_KEY_SPACE) {
	    tb_change_cell(tb.insert_x++, tb.insert_y, event.ch, TB_WHITE, TB_BLACK);
	    if(tb.insert_x >= tb.screen_width) {
		++tb.insert_y;
		tb.insert_x = 0;
	    }
	    buffer.push_back(event.ch);
	    present(tb.insert_x, tb.insert_y);
	} else if(event.key == TB_KEY_BACKSPACE2) {
	    if(tb.insert_y <= 0 && tb.insert_x <= 0) continue;
	    if(tb.insert_x == 0) {
		tb.insert_x = tb.screen_width - 1;
		tb_change_cell(tb.insert_x, --tb.insert_y, ' ', TB_WHITE, TB_BLACK);
	    } else {
		tb_change_cell(--tb.insert_x, tb.insert_y, ' ', TB_WHITE, TB_BLACK);
	    }
	    buffer.pop_back();
	    
	    present(tb.insert_x, tb.insert_y);
	}
    }

    return 0;
}
