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

    int size() const
    {
	return screen_width * screen_height;
    }

    void shift_right_at_insert(unsigned int file_size)
    {
	auto *insert = tb_cell_buffer() + (screen_width * insert_y) + insert_x;
	std::move(insert, insert+file_size
		  - (screen_width * insert_y + insert_x),
		  insert+1);
	tb_change_cell(insert_x, insert_y, ' ', TB_DEFAULT, TB_DEFAULT);
    }

    void return_at_insert(unsigned int file_size)
    {
	auto *insert = tb_cell_buffer() + (screen_width * insert_y) + insert_x;
	std::move(insert, insert+file_size
		  - (screen_width * insert_y + insert_x),
		  insert+(screen_width-insert_x));
	tb_change_cell(insert_x, insert_y, ' ', TB_DEFAULT, TB_DEFAULT);
    }

    ~TermboxApp() { tb_shutdown(); }
};

class BufferIterator {
private:
    std::list<char> &m_buffer;
    std::list<char>::iterator m_buff_iter;
    TermboxApp &m_tb;
    unsigned int m_screen_position = 0;
public:
    explicit BufferIterator(std::list<char> &buffer, TermboxApp &tb)
	: m_buffer{buffer}, m_buff_iter{buffer.begin()}, m_tb(tb)
    {
	++m_buff_iter;
    }

    auto& operator*()
    {
	return m_buff_iter;
    }

    bool at_end() const
    {
	return m_buff_iter == m_buffer.end();
    }

    auto screen_position() const
    {
	return m_screen_position;
    }

    bool operator++()
    {
	if(m_buff_iter == m_buffer.end() || m_tb.insert_y >= m_tb.screen_height) {
	    // If at end of screen, can't go right any more
	    return false;
	} else if(m_tb.insert_x >= m_tb.screen_width-1
		  && m_tb.insert_y < m_tb.screen_height-1) {
	    // If at the end of a line, bump cursor down to next line
	    ++m_tb.insert_y;
	    m_tb.insert_x = 0;
	} else {
	    // Normal case: just move cursor right one cell
	    ++m_tb.insert_x;
	}
	++m_buff_iter;
	++m_screen_position;
	return true;
    }

    bool operator--()
    {
	if(m_buff_iter == m_buffer.begin()
	   || (m_tb.insert_x <= 0 && m_tb.insert_y <= 0)) {
	    return false;
	} else if(m_tb.insert_x <= 0 && m_tb.insert_y > 0) {
	    auto prior = std::find_if(m_tb.line_ends.begin(), m_tb.line_ends.end(),
				      [this](const auto &end) {
					  return end[1] == this->m_tb.insert_y-1;
				      });
	    assert(prior != m_tb.line_ends.end());
	    auto[col, row] = *prior;
	    m_tb.insert_x = col;
	    m_tb.insert_y = row;
	    m_tb.line_ends.erase(prior);
	} else {
	    --m_tb.insert_x;
	}
	--m_buff_iter;
	--m_screen_position;
	return true;
    }
};

bool can_return(const std::list<char> &buffer, const TermboxApp &screen)
{
    return (screen.insert_y != screen.screen_height-1)
	&& screen.size() - buffer.size() > 0;
}

bool can_insert(const std::list<char> &buffer, const TermboxApp &screen)
{
    return screen.size() - buffer.size() > 0;
}

void insert(TermboxApp &screen, std::list<char> &buffer,
	    BufferIterator &inserter, char letter)
{
    buffer.insert(*inserter, letter);
    if(!inserter.at_end()) {
	screen.shift_right_at_insert(buffer.size());
    }
    tb_change_cell(screen.insert_x, screen.insert_y, letter, TB_DEFAULT, TB_DEFAULT);
    --*inserter;
    ++inserter;
}

int main()
{
    TermboxApp tb;
    tb_event event{};
    bool running = true;
    std::list<char> buffer;
    BufferIterator inserter(buffer, tb);
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
		  && can_insert(buffer, tb)) {
	    // User typed a character into the buffer
	    insert(tb, buffer, inserter, event.ch);
	    present(tb.insert_x, tb.insert_y);
	} else if(event.key == TB_KEY_ENTER && can_return(buffer, tb)) {
	    // User pressed Enter key to add a newline
	    insert(tb, buffer, inserter, '\n');
	    present(tb.insert_x, tb.insert_y);
	} else if(event.key == TB_KEY_BACKSPACE2 || event.key == TB_KEY_BACKSPACE) {
	      // User backspaced a character

	} else if(event.key == TB_KEY_ARROW_LEFT) {
	    // User wants to move back one character
	    --inserter;
	    present(tb.insert_x, tb.insert_y);
	} else if(event.key == TB_KEY_ARROW_RIGHT) {
	    // User wants to move forward one character
	    ++inserter;
	    present(tb.insert_x, tb.insert_y);
	}
    }

    return 0;
}
