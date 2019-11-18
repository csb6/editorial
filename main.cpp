#include <iostream>
#include "termbox.h"
#include <list>
#include <algorithm>
#include <array>

static inline void present(int insert_x, int insert_y);

class TermboxApp {
public:
    int screen_width, screen_height;
    int insert_x = 0;
    int insert_y = 0;
    // [column, row] (in the screen buffer)
    std::list<std::array<int,2>> line_ends;
    TermboxApp()
    {
	auto status = tb_init();
	if(status < 0) {
	    tb_shutdown();
	    std::clog << "Error: Termbox failed with code " << status << std::endl;
	    exit(1);
	}
	screen_width = tb_width();
	screen_height = tb_height();
    }

    // Re-layout/draw the screen to fit the window's current size
    void resize(const std::list<char> &buffer)
    {
	tb_clear();
	screen_width = tb_width();
	screen_height = tb_height();
	int row = 0;
	int col = 0;
	for(char each : buffer) {
	    if(col >= screen_width || each == '\n') {
		col = 0;
		++row;
	    }
	    if(each == '\n') {
		line_ends.push_back({col,row});
		continue;
	    }
	    tb_change_cell(col++, row, each, TB_DEFAULT, TB_DEFAULT);
	    if(row >= screen_height) {
		break;
	    }
	}
	insert_x = col;
	insert_y = (row >= screen_height) ? screen_height-1 : row;
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
    
    while(running && tb_poll_event(&event) != -1) {
	if(event.type == TB_EVENT_RESIZE) {
	    tb.resize(buffer);
	} else if(event.type != TB_EVENT_KEY) {
	    continue;
	} else if(event.key == TB_KEY_CTRL_C) {
	    running = false;
	} else if(event.ch >= '!' || event.key == TB_KEY_SPACE) {
	    tb_change_cell(tb.insert_x++, tb.insert_y, event.ch, TB_DEFAULT, TB_DEFAULT);
	    if(tb.insert_x >= tb.screen_width) {
		++tb.insert_y;
		tb.insert_x = 0;
	    }
	    buffer.push_back(event.ch);
	    present(tb.insert_x, tb.insert_y);
	} else if(event.key == TB_KEY_ENTER
		  && tb.insert_y + 1 < tb.screen_height) {
	    buffer.push_back('\n');
	    tb.line_ends.push_back({tb.insert_x, tb.insert_y});
	    tb.insert_x = 0;
	    present(tb.insert_x, ++tb.insert_y);
	} else if((event.key == TB_KEY_BACKSPACE2 || event.key == TB_KEY_BACKSPACE)
		  && (tb.insert_y > 0 || tb.insert_x > 0)) {
	    if(buffer.back() == '\n') {
	        auto prior = std::find_if(tb.line_ends.begin(), tb.line_ends.end(),
					  [&tb](const auto &end) {
					      return end[1] == tb.insert_y-1;
					  });
		assert(prior != tb.line_ends.end());
		auto[col, row] = *prior;
		tb.insert_x = col;
		tb.insert_y = row;
		tb.line_ends.erase(prior);
	    } else if(tb.insert_x == 0) {
		tb.insert_x = tb.screen_width - 1;
		tb_change_cell(tb.insert_x, --tb.insert_y, ' ', TB_DEFAULT, TB_DEFAULT);
	    } else {
		tb_change_cell(--tb.insert_x, tb.insert_y, ' ', TB_DEFAULT, TB_DEFAULT);
	    }

	    buffer.pop_back();
	    present(tb.insert_x, tb.insert_y);
	}
    }

    return 0;
}
