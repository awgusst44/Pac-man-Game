#include "Listener.hpp"

#include <cctype>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

namespace pacman {

#ifndef _WIN32
namespace {
termios g_savedTerminal;  // setările inițiale ale terminalului
}  // namespace
#endif

ConsoleListener::ConsoleListener() {
#ifndef _WIN32
    if (tcgetattr(STDIN_FILENO, &g_savedTerminal) == 0) {
        termios raw = g_savedTerminal;
        raw.c_lflag &= static_cast<tcflag_t>(~(ICANON | ECHO));  // fără buffer de linie, fără echo
        raw.c_cc[VMIN] = 0;                                      // read() nu blochează
        raw.c_cc[VTIME] = 0;
        rawModeActive_ = (tcsetattr(STDIN_FILENO, TCSANOW, &raw) == 0);
    }
#endif
}

ConsoleListener::~ConsoleListener() {
#ifndef _WIN32
    if (rawModeActive_) {
        tcsetattr(STDIN_FILENO, TCSANOW, &g_savedTerminal);
    }
#endif
}

Command ConsoleListener::translate(int key) const {
    if (key < 0 || key > 255) {
        return Command::None;
    }
    switch (std::tolower(key)) {
        case 'w': return Command::MoveUp;
        case 'a': return Command::MoveLeft;
        case 's': return Command::MoveDown;
        case 'd': return Command::MoveRight;
        case 'p': return Command::Pause;
        case 'q': return Command::Quit;
        default:  return Command::None;
    }
}

Command ConsoleListener::translateArrow(int code) const {
#ifdef _WIN32
    switch (code) {  // coduri _getch() după prefixul 0 / 224
        case 72: return Command::MoveUp;
        case 80: return Command::MoveDown;
        case 75: return Command::MoveLeft;
        case 77: return Command::MoveRight;
        default: return Command::None;
    }
#else
    switch (code) {  // secvența ESC [ A/B/C/D
        case 'A': return Command::MoveUp;
        case 'B': return Command::MoveDown;
        case 'D': return Command::MoveLeft;
        case 'C': return Command::MoveRight;
        default:  return Command::None;
    }
#endif
}

Command ConsoleListener::poll() {
    Command result = Command::None;

#ifdef _WIN32
    while (_kbhit()) {  // golește toate tastele apăsate de la ultimul tick
        const int key = _getch();
        const Command command =
            (key == 0 || key == 224) ? translateArrow(_getch()) : translate(key);
        if (command != Command::None) {
            result = command;
        }
    }
#else
    unsigned char ch = 0;
    while (read(STDIN_FILENO, &ch, 1) == 1) {
        Command command = Command::None;
        if (ch == 27) {  // ESC: posibilă săgeată
            unsigned char bracket = 0;
            unsigned char code = 0;
            if (read(STDIN_FILENO, &bracket, 1) == 1 && bracket == '[' &&
                read(STDIN_FILENO, &code, 1) == 1) {
                command = translateArrow(code);
            }
        } else {
            command = translate(ch);
        }
        if (command != Command::None) {
            result = command;
        }
    }
#endif
    return result;
}

}  // namespace pacman
