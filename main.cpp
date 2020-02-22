/*TODO:
[X] Fix highlighting so keywords aren't highlighted when typing inside a string
[ ] Add more C++ keywords, possibly more Markdown highlighting too
[X] Find more concise way to write highlighting code (maybe code gen?)
[ ] Consider using std::filebuf to represent the file in-memory
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
void(*highlight_mode)(int,int);

/**Writes as much of the given char grid to the screen as will fit;
   no line-wrapping (lines will be cut off when at edge)*/
void draw(const text_buffer_t &buffer, int start_row = 0)
{
    const int width = screen_width();
    const int height = screen_height();
    int row = 0;
    /* Starting drawing using content starting at the row currently at
       The top of the screen */
    auto curr_row = std::next(buffer.begin(), start_row);
    while(row < height && curr_row != buffer.end()) {
	int col = 0;
	auto letter = curr_row->begin();
	while(col < width && letter != curr_row->end()) {
	    if(std::isspace(*letter))
		set_cell(col++, row, *letter);
	    else
		set_cell(col++, row, *letter);
	    ++letter;
	}
	++row;
	++curr_row;
    }
    highlight_mode(start_row, height);
}

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

/**If necessary, move the visible text on screen up one line*/
static void scroll_up(int *cursor_y, int *top_visible_row,
                      const text_buffer_t &buffer)
{
    if(*cursor_y == -1) {
        // If going offscreen, scroll upwards
        --(*top_visible_row);
        *cursor_y = 0;
        screen_clear();
        draw(buffer, *top_visible_row);
    }
}

/**If necessary, move the visible text on screen down one line*/
static void scroll_down(int *cursor_y, int *top_visible_row,
                        text_buffer_t::iterator curr_row,
                        const text_buffer_t &buffer)
{
   if(*cursor_y == screen_height() && curr_row != buffer.end()) {
       // If going offscreen, scroll downwards
       ++(*top_visible_row);
       *cursor_y = screen_height() - 1;
       screen_clear();
       draw(buffer, *top_visible_row);
   }
}

constexpr bool ends_with(std::string_view text, std::string_view match)
{
    if(text.size() < match.size())
        return false;
    return text.substr(text.size() - match.size()) == match;
}


int main(int argc, char **argv)
{
    if(argc < 2) {
	std::printf("Usage: ./editorial </path/to/file>\n");
	return 1;
    }
    const char *filename = argv[1];
    text_buffer_t buffer{load(filename)};
    {
	/* Open the syntax-highlighting mode appropriate for the
	   file extension of the opened file */
	if(ends_with(filename, ".md"))
	    highlight_mode = markdown_mode;
	else if(ends_with(filename, ".cpp") || ends_with(filename, ".h"))
	    highlight_mode = cpp_mode;
	else
	    highlight_mode = text_mode;
    }

    Screen window;
    auto curr_row = buffer.begin();
    auto inserter = curr_row->begin();
    int cursor_x = 0;
    int cursor_y = 0;
    // The index of the row in the buffer at the top of the screen
    int top_visible_row = 0;
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
	    draw(buffer, top_visible_row);
	    set_cursor(cursor_x, cursor_y);
	    screen_present_resize();
	    continue;
	} else if(needs_redraw) {
	    screen_clear();
	    draw(buffer, top_visible_row);
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
	    write(0, 0, "Saved", Color::Yellow);
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    needs_redraw = true;
	    break;
	case Key_Enter:
	case Key_Enter2:
	    if(inserter == curr_row->begin() && curr_row->size() >= 1) {
		// If at beginning of line with at least some text,
		// newline goes before the cursor's row
		curr_row = std::next(buffer.insert(curr_row, text_row_t{}));
		inserter = curr_row->begin();
		cursor_x = 0;
		++cursor_y;
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
            scroll_down(&cursor_y, &top_visible_row, curr_row, buffer);
	    screen_clear();
	    draw(buffer, top_visible_row);
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
            scroll_up(&cursor_y, &top_visible_row, buffer);
	    screen_clear();
	    draw(buffer, top_visible_row);
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
                scroll_down(&cursor_y, &top_visible_row, curr_row, buffer);
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
                scroll_up(&cursor_y, &top_visible_row, buffer);
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
            scroll_up(&cursor_y, &top_visible_row, buffer);
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
            scroll_down(&cursor_y, &top_visible_row, curr_row, buffer);
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    break;
	}
	case Key_Tab:
	    inserter = curr_row->insert(inserter, TabSize, ' ');
	    std::advance(inserter, TabSize);
	    cursor_x += TabSize;
	    draw(buffer, top_visible_row);
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
	    break;
	default:
	    inserter = std::next(curr_row->insert(inserter, input));
	    ++cursor_x;
	    draw(buffer, top_visible_row);
	    set_cursor(cursor_x, cursor_y);
	    screen_present();
   	}
    }

    return 0;
}
