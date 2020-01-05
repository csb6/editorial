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

class BufferIterator {
private:
    std::list<char> &m_buffer;
    std::list<char>::iterator m_iter;
public:
    BufferIterator(std::list<char> &buffer)
	: m_buffer(buffer), m_iter(m_buffer.begin())
    {}
    auto& operator++()
    {
	if(m_iter != std::prev(m_buffer.end()))
	    ++m_iter;
	return m_iter;
    }
    auto& operator--()
    {
	if(m_iter != m_buffer.begin())
	    --m_iter;
	return m_iter;
    }
    auto& operator*() { return m_iter; }
};

class CursorIterator {
private:
    int x = 0;
    int y = 0;
public:
    CursorIterator() { tb_set_cursor(x, y); }
    void operator++()
    {
	if(x < tb_width()-1) {
	    ++x;
	    tb_set_cursor(x, y);
	} else {
	    next_line();
	}
    }
    void operator--()
    {
	if(x > 0) {
	    --x;
	    tb_set_cursor(x, y);
	} else {
	    prev_line();
	}
    }
    void next_line()
    {
	if(y < tb_height()-1) {
	    x = 0;
	    ++y;
	    tb_set_cursor(x, y);
	}
    }
    void prev_line()
    {
	if(y > 0) {
	    x = tb_width();
	    --y;
	    tb_set_cursor(x, y);
	}
    }
};

void draw(const std::list<char> &buffer)
{
    auto it = buffer.begin();
    int width = tb_width();
    int height = tb_height();
    for(int row = 0; row < height; ++row) {
	for(int col = 0; col < width; ++col) {
	    if(it == buffer.end())
		return;
	    if(*it == '\n') {
		++it;
		break;
	    }
	    tb_change_cell(col, row, *it, TB_DEFAULT, TB_DEFAULT);
	    ++it;
	}
    }
}

int main()
{
    Termbox tb;
    std::ifstream file("test.txt");
    std::list<char> buffer;
    while(file) {
	buffer.push_back(file.get());
    }
    if(buffer.size() >= 1 && buffer.back() == EOF)
	buffer.pop_back();
    draw(buffer);
    BufferIterator inserter(buffer);
    CursorIterator cursor;
    tb_present();
    tb_event curr_event;
    while(tb_poll_event(&curr_event) != -1) {
        if(curr_event.type != TB_EVENT_KEY)
	    continue;

	switch(curr_event.key) {
	case TB_KEY_CTRL_C:
	    // Exit program
	    return 0;
	case TB_KEY_ENTER:
	    buffer.insert(*inserter, '\n');
	    cursor.next_line();
	    tb_clear();
	    draw(buffer);
	    tb_present();
	    break;
	case TB_KEY_BACKSPACE:
	case TB_KEY_BACKSPACE2: {
	    auto new_iter = buffer.erase(std::prev(*inserter));
	    *inserter = new_iter;
	    --cursor;
	    tb_clear();
	    draw(buffer);
	    tb_present();
	    break;
	}
	case TB_KEY_ARROW_RIGHT:
	    ++cursor;
	    ++inserter;
	    tb_present();
	    break;
	case TB_KEY_ARROW_LEFT:
	    --cursor;
	    --inserter;
	    tb_present();
	    break;
	default: {
	    buffer.insert(*inserter, curr_event.ch);
	    ++cursor;
	    tb_clear();
	    draw(buffer);
	    tb_present();
	}
	}
    }

    return 0;
}
