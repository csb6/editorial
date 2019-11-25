#include <iostream>
#include "termbox.h"
#include <list>
#include <algorithm>
#include <array>

static inline void present(int insert_x, int insert_y)
{
    tb_set_cursor(insert_x, insert_y);
    tb_present();
}

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
    // Shift over all characters onscreen by one char to make an empty cell if
    // that cell is currently non-empty
    void maybe_shift_right(unsigned int buffer_size)
    {
	auto *curr = tb_cell_buffer() + screen_width * insert_y + insert_x;
	if(curr->ch != ' ') {
	    std::move(curr, curr+buffer_size
		      - (screen_width * insert_y + insert_x),
		      curr+1);
	}
    }
    ~TermboxApp() { tb_shutdown(); }
};

int main()
{   
    TermboxApp tb;
    tb_event event{};
    bool running = true;
    std::list<char> buffer;
    auto insert{buffer.end()};
    present(tb.insert_x, tb.insert_y);
    
    while(running && tb_poll_event(&event) != -1) {
	if(event.type == TB_EVENT_RESIZE) {
	    // User resizes terminal window
	    tb.resize(buffer);
	} else if(event.type != TB_EVENT_KEY) {
	    continue;
	} else if(event.key == TB_KEY_CTRL_C) {
	    // User would like to quit
	    running = false;
	} else if((event.ch >= '!' || event.key == TB_KEY_SPACE)
		  && (tb.insert_x < tb.screen_width-1 || tb.insert_y < tb.screen_height-1)) {
	    // User typed a character into the buffer
	    tb.maybe_shift_right(buffer.size());
	    tb_change_cell(tb.insert_x++, tb.insert_y, event.ch, TB_DEFAULT, TB_DEFAULT);
	    if(tb.insert_x >= tb.screen_width) {
		++tb.insert_y;
		tb.insert_x = 0;
	    }
	    buffer.insert(insert, event.ch);
	    assert(*std::prev(insert) == (char)event.ch);
	    present(tb.insert_x, tb.insert_y);
	} else if(event.key == TB_KEY_ENTER
		  && tb.insert_y + 1 < tb.screen_height) {
	    // User pressed Enter key to add a newline
	    buffer.insert(insert, '\n');
	    assert(*std::prev(insert) == '\n');
	    tb.line_ends.push_back({tb.insert_x, tb.insert_y});
	    tb.insert_x = 0;
	    present(tb.insert_x, ++tb.insert_y);
	} else if((event.key == TB_KEY_BACKSPACE2 || event.key == TB_KEY_BACKSPACE)
		  && (tb.insert_y > 0 || tb.insert_x > 0)) {
	    // User backspaced a character
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

	    --insert;
	    insert = buffer.erase(insert);
	    present(tb.insert_x, tb.insert_y);
	} else if(event.key == TB_KEY_ARROW_LEFT && tb.insert_x > 0) {
	    // User wants to move back one character
	    --insert;
	    present(--tb.insert_x, tb.insert_y);
	} else if(event.key == TB_KEY_ARROW_RIGHT && tb.insert_x < tb.screen_width
		  && insert != buffer.end()) {
	    // User wants to move forward one character
	    ++insert;
	    present(++tb.insert_x, tb.insert_y);
	}
    }

    return 0;
}
