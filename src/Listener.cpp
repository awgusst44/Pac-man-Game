#include "Listener.hpp"

#include <cctype>

#ifdef _WIN32
#include <conio.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

namespace pacman {

#ifndef _WIN32
namespace {

termios g_savedTerminal;  // setările inițiale ale terminalului

// Este un octet gata de citit? (nu blochează, nici când intrarea e o conductă, nu un terminal)
bool byteAvailable() {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(STDIN_FILENO, &set);
    timeval noWait{0, 0};
    return select(STDIN_FILENO + 1, &set, nullptr, nullptr, &noWait) > 0;
}

}  // namespace
#endif

ConsoleListener::ConsoleListener()
    : keys_{{'w', Command::MoveUp},
            {'a', Command::MoveLeft},
            {'s', Command::MoveDown},
            {'d', Command::MoveRight},
            {'p', Command::Pause},
            {'q', Command::Quit}} {
#ifdef _WIN32
    arrows_ = {{72, Command::MoveUp},    // coduri _getch() după prefixul 0 / 224
               {80, Command::MoveDown},
               {75, Command::MoveLeft},
               {77, Command::MoveRight}};
#else
    arrows_ = {{'A', Command::MoveUp},   // secvența ESC [ A/B/C/D
               {'B', Command::MoveDown},
               {'D', Command::MoveLeft},
               {'C', Command::MoveRight}};
#endif
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
    const auto found = keys_.find(std::tolower(key));
    return found != keys_.end() ? found->second : Command::None;
}

Command ConsoleListener::translateArrow(int code) const {
    const auto found = arrows_.find(code);
    return found != arrows_.end() ? found->second : Command::None;
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
    while (byteAvailable() && read(STDIN_FILENO, &ch, 1) == 1) {
        Command command = Command::None;
        if (ch == 27) {  // ESC: posibilă săgeată
            unsigned char bracket = 0;
            unsigned char code = 0;
            if (byteAvailable() && read(STDIN_FILENO, &bracket, 1) == 1 && bracket == '[' &&
                byteAvailable() && read(STDIN_FILENO, &code, 1) == 1) {
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
