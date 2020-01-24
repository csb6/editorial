#include <curses.h>
#include "screen.h"
#include <stdexcept>

Screen::Screen()
{
    // Character-at-a-time input, no echoing
    initscr();
    raw();
    noecho();
    keypad(stdscr, true);

    if(!has_colors())
	throw std::logic_error("Terminal doesn't support color");
    start_color();
    init_pair((short)Color::Red, COLOR_RED, COLOR_BLACK);
    init_pair((short)Color::Green, COLOR_GREEN, COLOR_BLACK);
    init_pair((short)Color::Yellow, COLOR_YELLOW, COLOR_BLACK);
    init_pair((short)Color::Blue, COLOR_BLUE, COLOR_BLACK);
    init_pair((short)Color::Magenta, COLOR_MAGENTA, COLOR_BLACK);
    init_pair((short)Color::Cyan, COLOR_CYAN, COLOR_BLACK);
    init_pair((short)Color::White, COLOR_WHITE, COLOR_BLACK);
}

Screen::~Screen() { endwin(); }

int screen_width() { return COLS; }

int screen_height() { return LINES; }

void screen_clear() { clear(); }

void screen_present() { refresh(); }

void screen_present_resize()
{
    screen_present();
    // Ignore next event, which will be an ERR
    get_ch();
}

int screen_get(int x, int y)
{
    return mvinch(y, x) & A_CHARTEXT;
}

int get_ch() { return getch(); }

class UsingColorPair {
private:
    const NCURSES_ATTR_T m_pair;
public:
    explicit UsingColorPair(Color fg)
	: m_pair(COLOR_PAIR(static_cast<NCURSES_ATTR_T>(fg)))
    {
	attron(m_pair);
    }
    NCURSES_ATTR_T get() const { return m_pair; }
    ~UsingColorPair() { attroff(m_pair); }
};

void set_cell(int x, int y, chtype ch, Color fg)
{
    UsingColorPair curr_color(fg);
    mvaddch(y, x, ch);
}

/**Writes the given text to the screen with optional coloring; text starts
   at the given coordinates, continuing from left to right; text wraps when
   it hits edge of screen*/
void write(int x, int y, const char *text, Color fg)
{
    UsingColorPair curr_color(fg);
    mvaddstr(y, x, text);
}

void set_cell_color(int x, int y, Color fg)
{
    auto character = mvinch(y, x);
    UsingColorPair curr_color(fg);
    if((character & A_COLOR) != curr_color.get()) {
        mvaddch(y, x, character & A_CHARTEXT);
    }
}

void set_cursor(int x, int y) { move(y, x); }
