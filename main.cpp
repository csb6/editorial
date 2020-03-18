/*TODO:
[X] Fix highlighting so keywords aren't highlighted when typing inside a string
[X] Add more C++ keywords, possibly more Markdown highlighting too
[X] Find more concise way to write highlighting code (maybe code gen?)
[ ] Implement undo-redo functionality (maybe can do with macros??)
[ ] Implement macro system that lets you record/play back keystrokes*/
#include <fstream>
#include <cstdio>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>
#include "screen.h"
#include "syntax-highlight.h"

constexpr std::size_t TabSize = 4; // in spaces

using text_row_t = std::vector<char>;
using text_buffer_t = std::list<text_row_t>;

// The current syntax highlighting mode; set when loading a file
void(*highlight_mode)(Screen&,int,int);

/**Writes as much of the given char grid to the screen as will fit;
   no line-wrapping (lines will be cut off when at edge)*/
void draw(Screen &window, const text_buffer_t &buffer, int start_row = 0)
{
    const int width = window.width();
    const int height = window.height();
    int row = 0;
    /* Starting drawing using content starting at the row currently at
       The top of the screen */
    auto curr_row = std::next(buffer.begin(), start_row);
    while(row < height && curr_row != buffer.end()) {
	int col = 0;
	auto letter = curr_row->begin();
	while(col < width && letter != curr_row->end()) {
	    if(std::isspace(*letter))
		window.set(col++, row, ' ');
	    else
		window.set(col++, row, *letter);
	    ++letter;
	}
	++row;
	++curr_row;
    }
    highlight_mode(window, start_row, height);
}

/**Creates a 2D grid of characters representing a given text file*/
text_buffer_t load(const char *filename)
{
    std::ifstream file(filename);
    if(!file)
	throw std::ifstream::failure("Could not open given file");
    text_buffer_t buffer;
    buffer.emplace_back();
    auto curr_row = buffer.begin();
    while(file) {
	char curr = file.get();
	if(curr == '\n') {
	    buffer.emplace_back();
            curr_row = std::prev(buffer.end());
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

/**If necessary, move the visible text on screen up one line*/
static void scroll_up(Screen &window, int *cursor_y, int *top_visible_row,
                      const text_buffer_t &buffer)
{
    if(*cursor_y == -1) {
        // If going offscreen, scroll upwards
        --(*top_visible_row);
        *cursor_y = 0;
        window.clear();
        draw(window, buffer, *top_visible_row);
    }
}

/**If necessary, move the visible text on screen down one line*/
static void scroll_down(Screen &window, int *cursor_y, int *top_visible_row,
                        text_buffer_t::iterator curr_row,
                        const text_buffer_t &buffer)
{
   if(*cursor_y == window.height() && curr_row != buffer.end()) {
       // If going offscreen, scroll downwards
       ++(*top_visible_row);
       *cursor_y = window.height() - 1;
       window.clear();
       draw(window, buffer, *top_visible_row);
   }
}

constexpr bool ends_with(std::string_view text, std::string_view match)
{
    if(text.size() < match.size())
        return false;
    return text.substr(text.size() - match.size()) == match;
}

class Cursor {
private:
    Screen &window;
    text_buffer_t &buffer;
public:
    int x;
    int y;
    text_buffer_t::iterator row_it;
    text_row_t::iterator col_it;

    Cursor(Screen &win, text_buffer_t &buf)
        : window(win), buffer(buf), row_it(buffer.begin()),
          col_it(row_it->begin())
    {
        // Cursor starts at (0, 0), upper left corner of screen
        set(0, 0);
    }

    void set(int x, int y)
    {
        this->x = x;
        this->y = y;
        window.set_cursor(x, y);
    }

    void refresh()
    {
        window.set_cursor(x, y);
    }

    void move_right(int amount = 1)
    {
        x += amount;
        std::advance(col_it, amount);
    }

    void move_left(int amount = 1)
    {
        x -= amount;
        std::advance(col_it, -amount);
    }

    void move_up()
    {
        --y;
        --row_it;
    }

    void move_down()
    {
        ++y;
        ++row_it;
    }

    void carriage_return()
    {
        col_it = row_it->begin();
        x = 0;
    }

    void move_line_end()
    {
        col_it = row_it->end();
        x = row_it->size();
    }
};


int main(int argc, char **argv)
{
    if(argc != 2) {
	std::printf("Usage: ./editorial </path/to/file>\n");
	return 1;
    }
    const char *filename = argv[1];
    text_buffer_t buffer{load(filename)};
    /* Open the syntax-highlighting mode appropriate for the
       file extension of the opened file */
    if(ends_with(filename, ".md"))
        highlight_mode = markdown_mode;
    else if(ends_with(filename, ".cpp") || ends_with(filename, ".h"))
        highlight_mode = cpp_mode;
    else if(ends_with(filename, ".s"))
        highlight_mode = mips_mode;
    else
        highlight_mode = text_mode;

    Screen window;
    Cursor cursor(window, buffer);
    // The index of the row in the buffer at the top of the screen
    int top_visible_row = 0;
    draw(window, buffer);
    cursor.refresh();
    window.present();
    // Flag to redraw screen on next tick
    bool needs_redraw = false;
    bool done = false;
    int input;
    while(!done && (input = window.get_input())) {
	if(input == Key_Resize) {
	    window.clear();
	    draw(window, buffer, top_visible_row);
	    cursor.refresh();
	    window.present_resize();
	    continue;
	} else if(needs_redraw) {
	    window.clear();
	    draw(window, buffer, top_visible_row);
	    cursor.refresh();
	    window.present();
	    needs_redraw = false;
	}

	switch(input) {
	case ctrl('c'):
	    // Exit program
	    done = true;
            break;
	case ctrl('s'):
	    // Save to disk
	    save(buffer, filename);
	    window.write(0, 0, "Saved", Color::Yellow);
	    cursor.refresh();
	    window.present();
	    needs_redraw = true;
	    break;
	case Key_Enter:
	case Key_Enter2:
	    if(cursor.col_it == cursor.row_it->begin() && !cursor.row_it->empty()) {
		// If at beginning of line with at least some text,
		// newline goes before the cursor's row
                cursor.row_it = buffer.emplace(cursor.row_it);
                cursor.move_down();
                cursor.carriage_return();
	    } else if(cursor.col_it == cursor.row_it->end()) {
		// If at end of line, newline goes after the cursor's row
                cursor.move_down();
                cursor.row_it = buffer.emplace(cursor.row_it);
                cursor.carriage_return();
	    } else {
		// If in middle of line, all text after newline goes to next line
		auto next_row = buffer.emplace(std::next(cursor.row_it));
		std::copy(cursor.col_it, cursor.row_it->end(),
			  std::back_inserter(*next_row));
		cursor.col_it = cursor.row_it->erase(cursor.col_it, cursor.row_it->end());
	        cursor.move_down();
                cursor.carriage_return();
	    }
            scroll_down(window, &cursor.y, &top_visible_row, cursor.row_it, buffer);
	    window.clear();
	    draw(window, buffer, top_visible_row);
            cursor.refresh();
	    window.present();
	    break;
	case Key_Backspace:
	case Key_Backspace2:
	    if(cursor.col_it != cursor.row_it->begin()) {
		// If line isn't empty, just remove the character
		cursor.col_it = cursor.row_it->erase(std::prev(cursor.col_it));
		--cursor.x;
	    } else if(cursor.row_it->empty() && cursor.row_it != buffer.begin()) {
		// If line is empty and not the first row, delete that line
                cursor.row_it = buffer.erase(cursor.row_it);
                cursor.move_up();
                cursor.move_line_end();
	    } else if(cursor.row_it != buffer.begin()) {
		// If deleting newline in front of line with text, move text of
		// that line to the end of the prior line
                cursor.move_up();
                auto old_row = std::next(cursor.row_it);
                auto old_len = old_row->size();
                std::move(old_row->begin(), old_row->end(),
                          std::back_inserter(*cursor.row_it));
                cursor.row_it = buffer.erase(old_row);
                // Move cursor to the front of the newly appended text
                cursor.move_line_end();
                cursor.move_left(old_len);
	    }
            scroll_up(window, &cursor.y, &top_visible_row, buffer);
	    window.clear();
	    draw(window, buffer, top_visible_row);
	    cursor.refresh();
	    window.present();
	    break;
	case Key_Right:
	    if(cursor.col_it != cursor.row_it->end()) {
		// Go right as long as there is text left to go over
		cursor.move_right();
	    } else if(std::next(cursor.row_it) != buffer.end()) {
		// Can't go right anymore at buffer end
                cursor.move_down();
                cursor.carriage_return();
                scroll_down(window, &cursor.y, &top_visible_row, cursor.row_it, buffer);
	    }
	    cursor.refresh();
	    window.present();
	    break;
	case Key_Left:
	    if(cursor.col_it != cursor.row_it->begin()) {
		// Go left as long as there is text left to go over
		cursor.move_left();
	    } else if(cursor.row_it != buffer.begin()) {
		// Can't go left anymore at buffer start
		cursor.move_up();
		cursor.move_line_end();
                scroll_up(window, &cursor.y, &top_visible_row, buffer);
	    }
	    cursor.refresh();
	    window.present();
	    break;
	case Key_Up: {
	    if(cursor.row_it == buffer.begin())
                break;
            cursor.move_up();
	    auto offset = std::min<unsigned long>(cursor.x, cursor.row_it->size());
            cursor.carriage_return();
            cursor.move_right(offset);
            scroll_up(window, &cursor.y, &top_visible_row, buffer);
	    cursor.refresh();
	    window.present();
	    break;
	}
	case Key_Down: {
	    if(std::next(cursor.row_it) == buffer.end())
		break;
            cursor.move_down();
            auto offset = std::min<unsigned long>(cursor.x, cursor.row_it->size());
            cursor.carriage_return();
            cursor.move_right(offset);
            scroll_down(window, &cursor.y, &top_visible_row, cursor.row_it, buffer);
	    cursor.refresh();
	    window.present();
	    break;
	}
	case Key_Tab:
	    cursor.col_it = cursor.row_it->insert(cursor.col_it, TabSize, ' ');
            cursor.move_right(TabSize);
	    draw(window, buffer, top_visible_row);
	    cursor.refresh();
	    window.present();
	    break;
	default:
	    cursor.col_it = std::next(cursor.row_it->insert(cursor.col_it, input));
	    ++cursor.x;
	    draw(window, buffer, top_visible_row);
	    cursor.refresh();
	    window.present();
   	}
    }

    return 0;
}
