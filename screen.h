#ifndef SCREEN_CURSES_H
#define SCREEN_CURSES_H

enum class Color : char {
  Red = 1, Green = 2, Yellow = 3, Blue = 4,
  Magenta = 5, Cyan = 6, White = 7
};

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

class Screen {
public:
    Screen();
    ~Screen();
};

int screen_width();
int screen_height();
void screen_clear();
void screen_present();
void screen_present_resize();
int screen_get(int x, int y);
int get_ch();
void set_cell(int x, int y, unsigned int ch, Color fg = Color::White);
void write(int x, int y, const char *text, Color fg = Color::White);
void set_cell_color(int x, int y, Color fg);
void set_cursor(int x, int y);
constexpr int ctrl(int c) { return c & 0x1f; }
#endif
