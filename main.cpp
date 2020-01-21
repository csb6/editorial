#include <fstream>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string_view>
#include "screen.h"
#include "syntax-highlight.h"

constexpr std::size_t TabSize = 4; // in spaces

using text_row_t = std::vector<char>;
using text_buffer_t = std::list<text_row_t>;

// The current syntax highlighting mode; set when loading a file
void(*highlight_mode)(int,int);

/**Writes as much of the given char grid to the screen as will fit;
   no line-wrapping (lines will be cut off when at edge)*/
void draw(text_buffer_t::const_iterator start,
	  text_buffer_t::const_iterator end, int startRow, int endRow)
{
    int col = 0;
    int row = startRow;
    const int width = screen_width();
    const int height = screen_height();
    for(auto row_buf = start; row_buf != end; ++row_buf) {
	if(row >= height) break;
	col = 0;
	for(char letter : *row_buf) {
	    if(col >= width) break;
	    else if(std::isspace(letter)) letter = ' ';
	    set_cell(col, row, letter);
	    ++col;
	}
	++row;
    }
    highlight_mode(startRow, endRow);
}

void draw(const text_buffer_t &buffer)
{
    draw(buffer.begin(), buffer.end(), 0, screen_height());
}

/**Writes the given text to the screen with optional coloring; text starts
   at the given coordinates, continuing from left to right; text wraps when
   it hits edge of screen*/
/*void write(int col, int row, std::string_view text, uint16_t fg = TB_DEFAULT,
	   uint16_t bg = TB_DEFAULT)
{
    const int width = tb_width();
    const int height = tb_height();
    if(col >= width || row >= height)
	return;

    for(char letter : text) {
	if(col >= width) {
	    if(++row >= height)
		break;
	    col = 0;
	}
	tb_change_cell(col, row, letter, fg, bg);
	++col;
    }
    }*/


/**Creates a 2D grid of characters representing a given text file*/
text_buffer_t load(const char *filename)
{
    std::ifstream file(filename);
    if(!file)
	throw std::ifstream::failure("Could not open given file");
    text_buffer_t buffer;
    buffer.push_back(text_row_t{});
    auto curr_row = buffer.begin();
    while(file) {
	char curr = file.get();
	if(curr == '\n') {
	    buffer.push_back(text_row_t{});
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

/**Write the buffer to disk as a text file*/
void save(const text_buffer_t &buffer, const char *filename)
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


int main(int argc, char **argv)
{
    const char *filename = "README.md";
    if(argc > 1) {
	filename = argv[1];
    }
    text_buffer_t buffer{load(filename)};
    {
	std::string_view name(filename);
	if(name.size() >= 3 && name.substr(name.size()-3) == ".md")
	    highlight_mode = markdown_mode;
	else if(name.size() >= 4 && name.substr(name.size()-4) == ".cpp")
	  highlight_mode = cpp_mode;
    }

    Screen window;
    auto curr_row = buffer.begin();
    auto inserter = curr_row->begin();
    int cursor_x = 0;
    int cursor_y = 0;
    draw(buffer);
    set_cursor(cursor_x, cursor_y);
    screen_present();
    // Flag to redraw screen on next tick
    bool needs_redraw = false;
    bool done = false;
    int input;
    while(!done && (input = get_ch())) {
	if(input == Key_Resize) {
	    screen_clear();
	    draw(buffer);
	    set_cursor(cursor_x, cursor_y);
	    screen_present_resize();
	    continue;
	} else if(needs_redraw) {
	    screen_clear();
	    draw(buffer);
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    needs_redraw = false;
	}

	switch(input) {
	case ctrl('c'):
	    // Exit program
	    return 0;
	case ctrl('s'):
	    // Save to disk
	    save(buffer, filename);
	    //write(0, 0, "Saved", TB_YELLOW);
	    screen_present();
	    needs_redraw = true;
	    break;
	case Key_Enter:
	case Key_Enter2:
	    if(inserter == curr_row->begin() && curr_row->size() >= 1) {
		// If at beginning of line with at least some text,
		// newline goes before the cursor's row
		curr_row = buffer.insert(curr_row, text_row_t{});
		inserter = curr_row->begin();
		cursor_x = 0;
	    } else if(inserter == curr_row->end()) {
		// If at end of line, newline goes after the cursor's row
		curr_row = buffer.insert(std::next(curr_row), text_row_t{});
		inserter = curr_row->begin();
		cursor_x = 0;
		++cursor_y;
	    } else {
		// If in middle of line, all text after newline goes to next line
		auto next_row = buffer.insert(std::next(curr_row), text_row_t{});
		std::copy(inserter, curr_row->end(),
			  std::back_inserter(*next_row));
		curr_row->erase(inserter, curr_row->end());
		curr_row = next_row;
		inserter = curr_row->begin();
		cursor_x = 0;
		++cursor_y;
	    }
	    screen_clear();
	    draw(buffer);
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    break;
	case Key_Backspace:
	case Key_Backspace2:
	    if(inserter != curr_row->begin()) {
		// If line isn't empty, just remove the character
		inserter = curr_row->erase(std::prev(inserter));
		--cursor_x;
	    } else if(curr_row->empty() && curr_row != buffer.begin()) {
		// If line is empty and not the first row, delete that line
		curr_row = std::prev(buffer.erase(curr_row));
		--cursor_y;
		inserter = curr_row->end();
		cursor_x = curr_row->size();
	    } else if(curr_row != buffer.begin()) {
		// If deleting newline in front of line with text, move text of
		// that line to the end of the prior line
		auto prior_row = std::prev(curr_row);
		std::move(curr_row->begin(), curr_row->end(),
			  std::back_inserter(*prior_row));
		--cursor_y;
		buffer.erase(curr_row);
		curr_row = prior_row;
		inserter = curr_row->end();
		cursor_x = curr_row->size();
	    }
	    screen_clear();
	    draw(buffer);
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    break;
	case Key_Right:
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
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    break;
	case Key_Left:
	    if(inserter != curr_row->begin()) {
		// Go left as long as there is text left to go over
		--inserter;
		--cursor_x;
	    } else if(curr_row != buffer.begin()) {
		// Can't go left anymore at buffer start
		--curr_row;
		--cursor_y;
		inserter = curr_row->end();
		cursor_x = curr_row->size();
	    }
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    break;
	case Key_Up: {
	    if(curr_row == buffer.begin())
		break;
	    unsigned long x_pos = std::distance(curr_row->begin(), inserter);
	    --curr_row;
	    x_pos = std::min(x_pos, curr_row->size());
	    inserter = std::next(curr_row->begin(), x_pos);
	    cursor_x = x_pos;
	    --cursor_y;
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    break;
	}
	case Key_Down: {
	    if(std::next(curr_row) == buffer.end())
		break;
	    unsigned long x_pos = std::distance(curr_row->begin(), inserter);
	    ++curr_row;
	    x_pos = std::min(x_pos, curr_row->size());
	    inserter = std::next(curr_row->begin(), x_pos);
	    cursor_x = x_pos;
	    ++cursor_y;
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    break;
	}
	case Key_Tab:
	    inserter = curr_row->insert(inserter, TabSize, ' ');
	    std::advance(inserter, 4);
	    cursor_x += TabSize;
	    screen_clear();
	    draw(buffer);
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    break;
	default:
	    inserter = std::next(curr_row->insert(inserter, input));
	    ++cursor_x;
	    screen_clear();
	    draw(buffer);
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
   	}
    }

    return 0;
}
