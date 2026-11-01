#pragma once

#include "Types.hpp"

namespace pacman {

// Ascultătorul: citește intrarea și o traduce în Command.
class Listener {
public:
    virtual ~Listener() = default;

    // Returnează comanda curentă (nu blochează); Command::None dacă nu s-a apăsat nimic.
    virtual Command poll() = 0;
};

// Implementare pentru tastatura în consolă (W/A/S/D sau săgeți, P, Q).
class ConsoleListener : public Listener {
public:
    ConsoleListener();   // pe Linux/MSYS2 pune terminalul în mod fără buffer
    ~ConsoleListener() override;
    ConsoleListener(const ConsoleListener&) = delete;
    ConsoleListener& operator=(const ConsoleListener&) = delete;

    Command poll() override;

private:
    Command translate(int key) const;       // taste normale
    Command translateArrow(int code) const; // taste speciale (săgeți)

    bool rawModeActive_{false};
};

}  // namespace pacman
