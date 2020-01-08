#include <iostream>
#include <fstream>
#include <list>
#include <algorithm>
#include "termbox.h"

class Termbox {
public:
    Termbox()
    {
	int status = tb_init();
	if(status < 0) {
	    throw std::runtime_error("Termbox could not start-up properly");
	}
    }
    ~Termbox() { tb_shutdown(); }
};

void draw(const std::list<std::list<char>> &buffer)
{
    int screen_size = tb_width() * tb_height();
    int col = 0;
    int row = 0;
    for(const auto &row_buf : buffer) {
	col = 0;
	for(char letter : row_buf) {
	    if(col * row >= screen_size)
		return;
	    tb_change_cell(col, row, letter, TB_DEFAULT, TB_DEFAULT);
	    ++col;
	}
	++row;
    }
}


int main()
{
    Termbox tb;
    const char *filename = "test.txt";
    std::ifstream file(filename);
    std::list<std::list<char>> buffer;
    buffer.push_back(std::list<char>{});
    auto curr_row = buffer.begin();
    while(file) {
	char curr = file.get();
	if(curr == '\n') {
	    buffer.push_back(std::list<char>{});
	    ++curr_row;
	} else {
	    curr_row->push_back(curr);
	}
    }
    if(buffer.size() >= 1 && curr_row->size() >= 1
       && curr_row->back() == EOF)
        curr_row->pop_back();
    draw(buffer);
    curr_row = buffer.begin();
    auto inserter = curr_row->begin();
    int cursor_x = 0;
    int cursor_y = 0;
    tb_set_cursor(cursor_x, cursor_y);
    tb_present();
    tb_event curr_event;
    while(tb_poll_event(&curr_event) != -1) {
        if(curr_event.type != TB_EVENT_KEY)
	    continue;

	switch(curr_event.key) {
	case TB_KEY_CTRL_C:
	    // Exit program
	    return 0;
	case TB_KEY_CTRL_S: {
	    // Save buffer
	    std::ofstream output_file(filename);
	    for(const auto &row_buf : buffer) {
		for(char letter : row_buf) {
		    output_file.put(letter);
		}
		if(row_buf != buffer.back())
		    output_file.put('\n');
	    }
	    break;
	}
	case TB_KEY_ENTER:
	    // TODO: properly append contents of split row to new row
	    if(inserter == curr_row->begin() && curr_row->size() >= 1) {
		curr_row = buffer.insert(curr_row, std::list<char>{});
		inserter = curr_row->begin();
		cursor_x = 0;
	    } else {
		curr_row = buffer.insert(std::next(curr_row), std::list<char>{});
		inserter = curr_row->begin();
		cursor_x = 0;
		++cursor_y;
	    }
	    tb_set_cursor(cursor_x, cursor_y);
	    tb_clear();
	    draw(buffer);
	    tb_present();
	    break;
	case TB_KEY_BACKSPACE:
	case TB_KEY_BACKSPACE2: {
	    if(inserter != curr_row->begin()) {
		inserter = curr_row->erase(std::prev(inserter));
		--cursor_x;
	    } else if(curr_row != buffer.begin()) {
		// TODO: properly append contents of erased row to prior row
		curr_row = buffer.erase(curr_row);
		--cursor_y;
		if(curr_row->size() >= 1) {
		    inserter = std::prev(curr_row->end());
		    cursor_x = curr_row->size() - 1;
		} else {
		    inserter = curr_row->begin();
		    cursor_x = 0;
		}
	    }
	    tb_set_cursor(cursor_x, cursor_y);
	    tb_clear();
	    draw(buffer);
	    tb_present();
	    break;
	}
	case TB_KEY_ARROW_RIGHT:
	    if(inserter != curr_row->end()) {
		++inserter;
		++cursor_x;
	    } else if(std::next(curr_row) != buffer.end()) {
		++curr_row;
		inserter = curr_row->begin();
		++cursor_y;
		cursor_x = 0;
	    }
	    tb_set_cursor(cursor_x, cursor_y);
	    tb_present();
	    break;
	case TB_KEY_ARROW_LEFT:
	    if(inserter != curr_row->begin()) {
		--inserter;
		--cursor_x;
	    } else if(curr_row != buffer.begin()) {
		--curr_row;
		--cursor_y;
		if(curr_row->size() >= 1) {
		    inserter = std::prev(curr_row->end());
		    cursor_x = curr_row->size() - 1;
		} else {
		    inserter = curr_row->begin();
		    cursor_x = 0;
		}
	    }
	    tb_set_cursor(cursor_x, cursor_y);
	    tb_present();
	    break;
	case TB_KEY_ARROW_UP: {
	    if(curr_row == buffer.begin())
		break;
	    auto x_pos = std::distance(curr_row->begin(), inserter);
	    --curr_row;
	    if(curr_row->size() >= 1)
		x_pos = std::min(x_pos, static_cast<long>(curr_row->size())-1);
	    else
		x_pos = 0;
	    inserter = std::next(curr_row->begin(), x_pos);
	    cursor_x = x_pos;
	    --cursor_y;
	    tb_set_cursor(cursor_x, cursor_y);
	    tb_present();
	    break;
	}
	case TB_KEY_ARROW_DOWN: {
	    if(std::next(curr_row) == buffer.end())
		break;
	    auto x_pos = std::distance(curr_row->begin(), inserter);
	    ++curr_row;
	    if(curr_row->size() >= 1)
		x_pos = std::min(x_pos, static_cast<long>(curr_row->size())-1);
	    else
		x_pos = 0;
	    inserter = std::next(curr_row->begin(), x_pos);
	    cursor_x = x_pos;
	    ++cursor_y;
	    tb_set_cursor(cursor_x, cursor_y);
	    tb_present();
	    break;
	}
	case TB_KEY_SPACE:
	    curr_row->insert(inserter, ' ');
	    ++cursor_x;
	    tb_set_cursor(cursor_x, cursor_y);
	    tb_clear();
	    draw(buffer);
	    tb_present();
	    break;
	default: {
	    curr_row->insert(inserter, curr_event.ch);
	    ++cursor_x;
	    tb_set_cursor(cursor_x, cursor_y);
	    tb_clear();
	    draw(buffer);
	    tb_present();
	}
	}
    }

    return 0;
}
