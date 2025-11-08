#include "terminal.hpp"
#include <iostream>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>

    #ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
    #define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
    #endif

    static DWORD g_oldMode;
    static HANDLE g_hStdin;
    static HANDLE g_hStdout;
#else
    #include <termios.h>
    #include <unistd.h>
    #include <sys/ioctl.h>

    static termios g_oldTermios;
#endif

namespace Terminal {

void init() {
#ifdef _WIN32
    g_hStdin  = GetStdHandle(STD_INPUT_HANDLE);
    g_hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

    // habilitar VT sequences para suportar ANSI
    DWORD outMode = 0;
    if (GetConsoleMode(g_hStdout, &outMode)) {
        outMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(g_hStdout, outMode);
    }

    DWORD inMode = 0;
    GetConsoleMode(g_hStdin, &inMode);
    g_oldMode = inMode;
    inMode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT);
    SetConsoleMode(g_hStdin, inMode);
#else
    tcgetattr(STDIN_FILENO, &g_oldTermios);
    termios raw = g_oldTermios;
    raw.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
#endif
}

void restore() {
#ifdef _WIN32
    SetConsoleMode(g_hStdin, g_oldMode);
#else
    tcsetattr(STDIN_FILENO, TCSANOW, &g_oldTermios);
#endif
    showCursor();
}

void clear() {
    std::cout << "\x1b[2J\x1b[H" << std::flush;
}

void moveCursor(int row, int col) {
    std::cout << "\x1b[" << row << ";" << col << "H" << std::flush;
}

void hideCursor() {
    std::cout << "\x1b[?25l" << std::flush;
}

void showCursor() {
    std::cout << "\x1b[?25h" << std::flush;
}

int width() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(g_hStdout, &info);
    return info.srWindow.Right - info.srWindow.Left + 1;
#else
    winsize w{};
    ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
    return w.ws_col ? w.ws_col : 80;
#endif
}

int height() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    GetConsoleScreenBufferInfo(g_hStdout, &info);
    return info.srWindow.Bottom - info.srWindow.Top + 1;
#else
    winsize w{};
    ioctl(STDIN_FILENO, TIOCGWINSZ, &w);
    return w.ws_row ? w.ws_row : 24;
#endif
}

KeyEvent readKey() {
#ifdef _WIN32
    int c = _getch();
    if (c == 0 || c == 224) {
        int c2 = _getch();
        switch (c2) {
            case 72: return {Key::Up, 0};
            case 80: return {Key::Down, 0};
            case 75: return {Key::Left, 0};
            case 77: return {Key::Right, 0};
            case 73: return {Key::PageUp, 0};     // PgUp
            case 81: return {Key::PageDown, 0};   // PgDown
            default: return {Key::Unknown, 0};
        }
    }

    // não-extendido cai no switch final
#else
    unsigned char c;
    if (read(STDIN_FILENO, &c, 1) <= 0) {
        return {Key::Unknown, 0};
    }

    if (c == '\x1b') { // sequência ANSI
        char seq[3];

        // lê o próximo byte
        if (read(STDIN_FILENO, &seq[0], 1) <= 0)
            return {Key::Esc, 0};

        if (seq[0] == '[') {
            // lê o segundo byte
            if (read(STDIN_FILENO, &seq[1], 1) <= 0)
                return {Key::Esc, 0};

            switch (seq[1]) {
                case 'A': return {Key::Up, 0};
                case 'B': return {Key::Down, 0};
                case 'C': return {Key::Right, 0};
                case 'D': return {Key::Left, 0};

                // PageUp:  ESC [ 5 ~
                // PageDown:ESC [ 6 ~
                case '5':
                case '6':
                    if (read(STDIN_FILENO, &seq[2], 1) <= 0)
                        return {Key::Unknown, 0};
                    if (seq[2] == '~') {
                        if (seq[1] == '5') return {Key::PageUp, 0};
                        if (seq[1] == '6') return {Key::PageDown, 0};
                    }
                    break;
            }
        }

        return {Key::Unknown, 0};
    }
#endif

    // comum (ambos)
    switch (c) {
        case '\r':
        case '\n': return {Key::Enter, 0};
        case 27:   return {Key::Esc, 0};
        case '\t': return {Key::Tab, 0};
        case 8:
        case 127:  return {Key::Backspace, 0};
        default:
            if (c >= 32 && c <= 126)
                return {Key::Character, static_cast<char>(c)};
            return {Key::Unknown, 0};
    }
}


} // namespace Terminal
