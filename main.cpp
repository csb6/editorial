#include <fstream>
#include <list>
#include <algorithm>
#include <iterator>
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
    int col = 0;
    int row = 0;
    const int width = tb_width();
    const int height = tb_height();
    for(const auto &row_buf : buffer) {
	if(row >= height) break;
	col = 0;
	for(char letter : row_buf) {
	    if(col >= width) break;
	    tb_change_cell(col, row, letter, TB_DEFAULT, TB_DEFAULT);
	    ++col;
	}
	++row;
    }
}

std::list<std::list<char>> load(const char *filename)
{
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
    return buffer;
}

void save(const std::list<std::list<char>> &buffer, const char *filename)
{
    std::ofstream output_file(filename);
    for(const auto &row_buf : buffer) {
	for(char letter : row_buf) {
	    output_file.put(letter);
	}
	if(row_buf != buffer.back())
	    output_file.put('\n');
    }
}


int main()
{
    Termbox tb;
    const char *filename = "test.txt";
    std::list<std::list<char>> buffer{load(filename)};

    auto curr_row = buffer.begin();
    auto inserter = curr_row->begin();
    int cursor_x = 0;
    int cursor_y = 0;
    tb_set_cursor(cursor_x, cursor_y);
    draw(buffer);
    tb_present();
    tb_event curr_event;
    while(tb_poll_event(&curr_event) != -1) {
	if(curr_event.type == TB_EVENT_RESIZE) {
	    tb_clear();
	    draw(buffer);
	    tb_present();
	    continue;
        } else if(curr_event.type != TB_EVENT_KEY)
	    continue;

	switch(curr_event.key) {
	case TB_KEY_CTRL_C:
	    // Exit program
	    return 0;
	case TB_KEY_CTRL_S: {
	    // Save to disk
	    save(buffer, filename);
	    break;
	}
	case TB_KEY_ENTER:
	    if(inserter == curr_row->begin() && curr_row->size() >= 1) {
		// If at beginning of line with at least some text,
		// newline goes before the cursor's row
		curr_row = buffer.insert(curr_row, std::list<char>{});
		inserter = curr_row->begin();
		cursor_x = 0;
	    } else if(inserter == curr_row->end()) {
		// Newline goes after the cursor's row
		curr_row = buffer.insert(std::next(curr_row), std::list<char>{});
		inserter = curr_row->begin();
		cursor_x = 0;
		++cursor_y;
	    } else {
		auto next_row = buffer.insert(std::next(curr_row), std::list<char>{});
		std::copy(inserter, curr_row->end(),
			  std::back_inserter(*next_row));
		curr_row->erase(inserter, curr_row->end());
		curr_row = next_row;
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
		// If line isn't empty, just remove the character
		inserter = curr_row->erase(std::prev(inserter));
		--cursor_x;
	    } else if(curr_row->empty() && curr_row != buffer.begin()) {
		// If line is empty and not the first row, delete that line
		curr_row = std::prev(buffer.erase(curr_row));
		--cursor_y;
		if(curr_row->size() >= 1) {
		    inserter = std::prev(curr_row->end());
		    cursor_x = curr_row->size() - 1;
		} else {
		    inserter = curr_row->begin();
		    cursor_x = 0;
		}
	    } else if(curr_row != buffer.begin()){
		// If deleting newline in front of line with text, move text of
		// that line to the end of the prior line
		auto prior_row = std::prev(curr_row);
		std::move(curr_row->begin(), curr_row->end(),
			  std::back_inserter(*prior_row));
		--cursor_y;
		buffer.erase(curr_row);
		curr_row = prior_row;
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
		// Go right as long as there is text left to go over
		++inserter;
		++cursor_x;
	    } else if(std::next(curr_row) != buffer.end()) {
		// Can't go right anymore at buffer end
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
		// Go left as long as there is text left to go over
		--inserter;
		--cursor_x;
	    } else if(curr_row != buffer.begin()) {
		// Can't go left anymore at buffer start
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
	    unsigned long x_pos = std::distance(curr_row->begin(), inserter);
	    --curr_row;
	    if(curr_row->size() >= 1)
		x_pos = std::min(x_pos, curr_row->size()-1);
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
	    unsigned long x_pos = std::distance(curr_row->begin(), inserter);
	    ++curr_row;
	    if(curr_row->size() >= 1)
		x_pos = std::min(x_pos, curr_row->size()-1);
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
