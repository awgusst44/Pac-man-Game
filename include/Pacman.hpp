#pragma once

#include <iosfwd>

#include "Enums.hpp"
#include "Position.hpp"

namespace pacman {

struct Pacman {
    Position position{};
    Position spawn{};
    Direction direction{Direction::None};
    Direction nextDirection{Direction::None};  // direcția cerută de jucător
    int lives{3};

    Pacman();                                               // implicit: 3 vieți, poziția (0, 0)
    explicit Pacman(const Position& spawn, int lives = 3);  // cu parametri
    Pacman(const Pacman& other);                            // de copiere
    Pacman& operator=(const Pacman& other);                 // de atribuire

    bool operator==(const Pacman& other) const;
    bool operator!=(const Pacman& other) const;

    void reset(int initialLives);  // poziție de start + vieți inițiale
    void respawn();                // revine la start, păstrează viețile
    void requestDirection(Direction dir);
    void loseLife();
    bool isAlive() const;
};

// Format:  Pacman pos=(1,2) spawn=(1,2) dir=Left next=Up lives=3
std::ostream& operator<<(std::ostream& out, const Pacman& pacman);
std::istream& operator>>(std::istream& in, Pacman& pacman);

}  // namespace pacman
