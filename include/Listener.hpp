#pragma once

#include "Types.hpp"

namespace pacman {

class Listener {
public:
    virtual ~Listener() = default;

    virtual Command poll() = 0;
};

class ConsoleListener : public Listener {
public:
    Command poll() override;

private:
    Command translate(int key) const;
};

}  