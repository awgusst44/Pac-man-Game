// Biblioteca pacman_ghost: clasa Ghost.
#include "Ghost.hpp"

#include <istream>
#include <ostream>

#include "StreamParsing.hpp"

namespace pacman {

Ghost::Ghost()
    : type(GhostType::Blinky),
      mode(GhostMode::Scatter),
      position(),
      spawn(),
      direction(Direction::None),
      frightenedTicks(0) {}

Ghost::Ghost(GhostType type, const Position& spawn)
    : type(type),
      mode(GhostMode::Scatter),
      position(spawn),
      spawn(spawn),
      direction(Direction::None),
      frightenedTicks(0) {}

Ghost::Ghost(const Ghost& other)
    : type(other.type),
      mode(other.mode),
      position(other.position),
      spawn(other.spawn),
      direction(other.direction),
      frightenedTicks(other.frightenedTicks) {}

Ghost& Ghost::operator=(const Ghost& other) {
    if (this != &other) {
        type = other.type;
        mode = other.mode;
        position = other.position;
        spawn = other.spawn;
        direction = other.direction;
        frightenedTicks = other.frightenedTicks;
    }
    return *this;
}

bool Ghost::operator==(const Ghost& other) const {
    return type == other.type && mode == other.mode && position == other.position &&
           spawn == other.spawn && direction == other.direction &&
           frightenedTicks == other.frightenedTicks;
}
bool Ghost::operator!=(const Ghost& other) const { return !(*this == other); }

void Ghost::reset() {
    position = spawn;
    direction = Direction::None;
    mode = GhostMode::Scatter;
    frightenedTicks = 0;
}

void Ghost::frighten(int ticks) {
    if (mode == GhostMode::Eaten) {
        return;  // o fantomă mâncată nu mai poate fi speriată
    }
    mode = GhostMode::Frightened;
    frightenedTicks = ticks;
    direction = opposite(direction);
}

void Ghost::tick() {
    if (mode != GhostMode::Frightened) {
        return;
    }
    if (--frightenedTicks <= 0) {
        frightenedTicks = 0;
        mode = GhostMode::Chase;
    }
}

void Ghost::markEaten() {
    mode = GhostMode::Eaten;
    frightenedTicks = 0;
}

bool Ghost::isVulnerable() const { return mode == GhostMode::Frightened; }

bool Ghost::isDangerous() const {
    return mode == GhostMode::Chase || mode == GhostMode::Scatter;
}

std::ostream& operator<<(std::ostream& out, const Ghost& ghost) {
    return out << "Ghost type=" << ghost.type << " mode=" << ghost.mode << " pos=" << ghost.position
               << " spawn=" << ghost.spawn << " dir=" << ghost.direction
               << " frightened=" << ghost.frightenedTicks;
}

std::istream& operator>>(std::istream& in, Ghost& ghost) {
    using namespace detail;
    Ghost tmp;
    if (expectKeyword(in, "Ghost") && field(in, "type") && static_cast<bool>(in >> tmp.type) &&
        field(in, "mode") && static_cast<bool>(in >> tmp.mode) && field(in, "pos") &&
        static_cast<bool>(in >> tmp.position) && field(in, "spawn") &&
        static_cast<bool>(in >> tmp.spawn) && field(in, "dir") &&
        static_cast<bool>(in >> tmp.direction) && intField(in, "frightened", tmp.frightenedTicks)) {
        ghost = tmp;
    }
    return in;
}

}  // namespace pacman
