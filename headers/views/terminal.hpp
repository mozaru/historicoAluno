#ifndef _TERMINAL_HPP_
#define _TERMINAL_HPP_

#include <optional>

enum class Key {
    Up,
    Down,
    Left,
    Right,
    Enter,
    Esc,
    Tab,
    Backspace,
    Character,
    PageUp,
    PageDown,
    Unknown
};

struct KeyEvent {
    Key key;
    char ch; // válido só se key == Character
};

namespace Terminal {
    void init();
    void restore();

    void clear();
    void moveCursor(int row, int col);
    void hideCursor();
    void showCursor();

    int width();
    int height();

    KeyEvent readKey(); // bloqueante, lê setas, enter, esc, etc.
}

#endif