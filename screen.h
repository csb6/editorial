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
/**Ex: ctrl('c') -> 'Ctrl-c'; works in switch statements*/
constexpr int ctrl(int c) { return c & 0x1f; }
/**Manages NCurses setup/cleanup; checks that terminal supports colors;
   contains all functions for modifying screen*/
class Screen {
public:
    Screen();
    ~Screen();
    int width() const;
    int height() const;
    void clear();
    /**Sync the screen buffer with the terminal display*/
    void present();
    /**Present after a resize (needed to catch error code after resize)*/
    void present_resize();
    /**Get a character from the current screen buffer*/
    int get(int x, int y) const;
    /**Get one character of user input (includes events like scrolling/ctrl keys)*/
    int get_input();
    void set(int x, int y, unsigned int ch, Color fg = Color::Default);
    void set_color(int x, int y, Color fg);
    /**Move the cursor to a position onscreen. Doesn't require a subsequent
       screen_present() call to show up onscreen.*/
    void set_cursor(int x, int y);
    /**Print text to screen starting from (x, y), left-to-right*/
    void write(int x, int y, const char *text, Color fg = Color::Default);
};
#endif
