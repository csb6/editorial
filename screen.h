#ifndef SCREEN_CURSES_H
#define SCREEN_CURSES_H

enum class Color : char {
    Red = 1, Green = 2, Yellow = 3, Blue = 4,
    Magenta = 5, Cyan = 6, White = 7, Default = 8
};

/**NCurses constants that can be used without #include-ing curses.h*/
constexpr int Key_Tab = '\t';
constexpr int Key_Backspace = 0407;
constexpr int Key_Backspace2 = 127;
constexpr int Key_Enter = 0527;
constexpr int Key_Enter2 = '\n';
constexpr int Key_Up = 0403;
constexpr int Key_Down = 0402;
constexpr int Key_Left = 0404;
constexpr int Key_Right = 0405;
constexpr int Key_Resize = 0632;
constexpr int ErrCode = -1;
/**Ex: ctrl('c') -> 'Ctrl-c'*/
constexpr int ctrl(int c) { return c & 0x1f; }

/**Manages NCurses setup/cleanup; checks that terminal supports colors*/
class Screen {
public:
    Screen();
    ~Screen();
};

/**Access/modify the terminal screen*/
int screen_width();
int screen_height();
void screen_clear();
/**Sync the screen buffer with the terminal display*/
void screen_present();
/**Present after a resize (needed to catch error code after resize)*/
void screen_present_resize();
/**Get a character from the current screen buffer*/
int screen_get(int x, int y);
/**Get one character of user input*/
int get_ch();
void set_cell(int x, int y, unsigned int ch, Color fg = Color::Default);
/**Print text to screen starting from (x, y), left-to-right*/
void write(int x, int y, const char *text, Color fg = Color::Default);
void set_cell_color(int x, int y, Color fg);
void set_cursor(int x, int y);
#endif
