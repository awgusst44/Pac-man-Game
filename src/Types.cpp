#include "Types.hpp"

#include <algorithm>

namespace pacman {

// ---------- funcții ajutătoare pentru enum-uri ----------

Direction opposite(Direction dir) {
    switch (dir) {
        case Direction::Up:    return Direction::Down;
        case Direction::Down:  return Direction::Up;
        case Direction::Left:  return Direction::Right;
        case Direction::Right: return Direction::Left;
        default:               return Direction::None;
    }
}

Position directionOffset(Direction dir) {
    switch (dir) {
        case Direction::Up:    return {0, -1};
        case Direction::Down:  return {0, 1};
        case Direction::Left:  return {-1, 0};
        case Direction::Right: return {1, 0};
        default:               return {0, 0};
    }
}

Direction directionFromCommand(Command command) {
    switch (command) {
        case Command::MoveUp:    return Direction::Up;
        case Command::MoveDown:  return Direction::Down;
        case Command::MoveLeft:  return Direction::Left;
        case Command::MoveRight: return Direction::Right;
        default:                 return Direction::None;
    }
}

const char* toString(Direction dir) {
    switch (dir) {
        case Direction::Up:    return "Up";
        case Direction::Down:  return "Down";
        case Direction::Left:  return "Left";
        case Direction::Right: return "Right";
        default:               return "None";
    }
}

const char* toString(GameState state) {
    switch (state) {
        case GameState::Menu:    return "Menu";
        case GameState::Running: return "Running";
        case GameState::Paused:  return "Paused";
        case GameState::Won:     return "Won";
        case GameState::Lost:    return "Lost";
    }
    return "Unknown";
}

const char* toString(GhostMode mode) {
    switch (mode) {
        case GhostMode::Chase:      return "Chase";
        case GhostMode::Scatter:    return "Scatter";
        case GhostMode::Frightened: return "Frightened";
        case GhostMode::Eaten:      return "Eaten";
    }
    return "Unknown";
}

// ---------- Position ----------

Position Position::moved(Direction dir) const {
    const Position offset = directionOffset(dir);
    return {x + offset.x, y + offset.y};
}

int Position::distanceSquared(const Position& other) const {
    const int dx = x - other.x;
    const int dy = y - other.y;
    return dx * dx + dy * dy;
}

// ---------- Pacman ----------

void Pacman::reset(int initialLives) {
    lives = initialLives;
    respawn();
}

void Pacman::respawn() {
    position = spawn;
    direction = Direction::None;
    nextDirection = Direction::None;
}

void Pacman::requestDirection(Direction dir) {
    nextDirection = dir;
}

void Pacman::loseLife() {
    if (lives > 0) {
        --lives;
    }
}

bool Pacman::isAlive() const {
    return lives > 0;
}

// ---------- Ghost ----------

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

bool Ghost::isVulnerable() const {
    return mode == GhostMode::Frightened;
}

bool Ghost::isDangerous() const {
    return mode == GhostMode::Chase || mode == GhostMode::Scatter;
}

// ---------- Board ----------

Board::Board(int w, int h, CellType fill)
    : width(std::max(w, 0)), height(std::max(h, 0)) {
    cells.assign(static_cast<std::size_t>(width) * static_cast<std::size_t>(height), fill);
}

CellType Board::get(int x, int y) const {
    return inBounds(x, y) ? at(x, y) : CellType::Wall;
}

Position Board::wrap(const Position& position) const {
    if (width <= 0 || height <= 0) {
        return position;
    }
    return {((position.x % width) + width) % width, ((position.y % height) + height) % height};
}

bool Board::isWalkable(const Position& position, bool allowGhostHouse) const {
    if (!inBounds(position.x, position.y)) {
        return false;
    }
    const CellType cell = at(position.x, position.y);
    if (cell == CellType::Wall) {
        return false;
    }
    return allowGhostHouse || cell != CellType::GhostHouse;
}

int Board::count(CellType type) const {
    return static_cast<int>(std::count(cells.begin(), cells.end(), type));
}

// ---------- Score ----------

void Score::add(int points) {
    current += points;
    best = std::max(best, current);
}

void Score::reset() {
    current = 0;
}

// ---------- GameConfig ----------

bool GameConfig::isValid() const {
    return initialLives > 0 && frightenedDuration > 0 && dotPoints >= 0 && powerPelletPoints >= 0 &&
           ghostPoints >= 0 && scatterTicks > 0 && chaseTicks > 0 && ghostSpeedDivider > 0 &&
           tickMillis > 0;
}

}  // namespace pacman
