#pragma once

#include <iosfwd>

#include "Enums.hpp"
#include "Position.hpp"

namespace pacman {

struct Ghost {
    GhostType type{GhostType::Blinky};
    GhostMode mode{GhostMode::Scatter};
    Position position{};
    Position spawn{};
    Direction direction{Direction::None};
    int frightenedTicks{0};  // tick-uri rămase în modul Frightened

    Ghost();                                         // implicit: Blinky la (0, 0)
    Ghost(GhostType type, const Position& spawn);    // cu parametri
    Ghost(const Ghost& other);                       // de copiere
    Ghost& operator=(const Ghost& other);            // de atribuire

    bool operator==(const Ghost& other) const;
    bool operator!=(const Ghost& other) const;

    void reset();                 // revine la start
    void frighten(int ticks);     // devine vulnerabilă și își inversează direcția
    void tick();                  // scade timerul de vulnerabilitate
    void markEaten();             // a fost mâncată de Pacman
    bool isVulnerable() const;    // mode == Frightened
    bool isDangerous() const;     // mode == Chase sau Scatter
};

// Format:  Ghost type=Blinky mode=Scatter pos=(1,2) spawn=(1,2) dir=None frightened=0
std::ostream& operator<<(std::ostream& out, const Ghost& ghost);
std::istream& operator>>(std::istream& in, Ghost& ghost);

}  // namespace pacman
