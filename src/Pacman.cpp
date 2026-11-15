// Biblioteca pacman_pacman: clasa Pacman.
#include "Pacman.hpp"

#include <algorithm>
#include <istream>
#include <ostream>

#include "StreamParsing.hpp"

namespace pacman {

Pacman::Pacman()
    : position(), spawn(), direction(Direction::None), nextDirection(Direction::None), lives(3) {}

Pacman::Pacman(const Position& spawn, int lives)
    : position(spawn),
      spawn(spawn),
      direction(Direction::None),
      nextDirection(Direction::None),
      lives(std::max(lives, 0)) {}

Pacman::Pacman(const Pacman& other)
    : position(other.position),
      spawn(other.spawn),
      direction(other.direction),
      nextDirection(other.nextDirection),
      lives(other.lives) {}

Pacman& Pacman::operator=(const Pacman& other) {
    if (this != &other) {
        position = other.position;
        spawn = other.spawn;
        direction = other.direction;
        nextDirection = other.nextDirection;
        lives = other.lives;
    }
    return *this;
}

bool Pacman::operator==(const Pacman& other) const {
    return position == other.position && spawn == other.spawn && direction == other.direction &&
           nextDirection == other.nextDirection && lives == other.lives;
}
bool Pacman::operator!=(const Pacman& other) const { return !(*this == other); }

void Pacman::reset(int initialLives) {
    lives = initialLives;
    respawn();
}

void Pacman::respawn() {
    position = spawn;
    direction = Direction::None;
    nextDirection = Direction::None;
}

void Pacman::requestDirection(Direction dir) { nextDirection = dir; }

void Pacman::loseLife() {
    if (lives > 0) {
        --lives;
    }
}

bool Pacman::isAlive() const { return lives > 0; }

std::ostream& operator<<(std::ostream& out, const Pacman& pacman) {
    return out << "Pacman pos=" << pacman.position << " spawn=" << pacman.spawn
               << " dir=" << pacman.direction << " next=" << pacman.nextDirection
               << " lives=" << pacman.lives;
}

std::istream& operator>>(std::istream& in, Pacman& pacman) {
    using namespace detail;
    Pacman tmp;
    if (expectKeyword(in, "Pacman") && field(in, "pos") && static_cast<bool>(in >> tmp.position) &&
        field(in, "spawn") && static_cast<bool>(in >> tmp.spawn) && field(in, "dir") &&
        static_cast<bool>(in >> tmp.direction) && field(in, "next") &&
        static_cast<bool>(in >> tmp.nextDirection) && intField(in, "lives", tmp.lives)) {
        pacman = tmp;
    }
    return in;
}

}  // namespace pacman
