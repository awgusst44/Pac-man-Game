// Biblioteca pacman_enums: enum-urile jocului și funcțiile lor ajutătoare.
#include "Enums.hpp"

#include <istream>
#include <ostream>

#include "StreamParsing.hpp"

namespace pacman {

Direction opposite(Direction dir) {
    switch (dir) {
        case Direction::Up:    return Direction::Down;
        case Direction::Down:  return Direction::Up;
        case Direction::Left:  return Direction::Right;
        case Direction::Right: return Direction::Left;
        default:               return Direction::None;
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

std::string toString(Direction dir) {
    switch (dir) {
        case Direction::Up:    return "Up";
        case Direction::Down:  return "Down";
        case Direction::Left:  return "Left";
        case Direction::Right: return "Right";
        default:               return "None";
    }
}

std::string toString(GameState state) {
    switch (state) {
        case GameState::Menu:    return "Menu";
        case GameState::Running: return "Running";
        case GameState::Paused:  return "Paused";
        case GameState::Won:     return "Won";
        case GameState::Lost:    return "Lost";
    }
    return "Unknown";
}

std::string toString(GhostType type) {
    switch (type) {
        case GhostType::Blinky: return "Blinky";
        case GhostType::Pinky:  return "Pinky";
        case GhostType::Inky:   return "Inky";
        case GhostType::Clyde:  return "Clyde";
    }
    return "Unknown";
}

std::string toString(GhostMode mode) {
    switch (mode) {
        case GhostMode::Chase:      return "Chase";
        case GhostMode::Scatter:    return "Scatter";
        case GhostMode::Frightened: return "Frightened";
        case GhostMode::Eaten:      return "Eaten";
    }
    return "Unknown";
}

char toChar(CellType cell) {
    switch (cell) {
        case CellType::Wall:        return '#';
        case CellType::Dot:         return '.';
        case CellType::PowerPellet: return 'o';
        case CellType::GhostHouse:  return '-';
        case CellType::Empty:       return ' ';
    }
    return ' ';
}

bool cellFromChar(char symbol, CellType& cell) {
    switch (symbol) {
        case '#': cell = CellType::Wall;        return true;
        case '.': cell = CellType::Dot;         return true;
        case 'o': cell = CellType::PowerPellet; return true;
        case '-': cell = CellType::GhostHouse;  return true;
        case ' ': cell = CellType::Empty;       return true;
        default:  return false;
    }
}

std::ostream& operator<<(std::ostream& out, Direction dir) { return out << toString(dir); }
std::ostream& operator<<(std::ostream& out, CellType cell) { return out << toChar(cell); }
std::ostream& operator<<(std::ostream& out, GameState state) { return out << toString(state); }
std::ostream& operator<<(std::ostream& out, GhostType type) { return out << toString(type); }
std::ostream& operator<<(std::ostream& out, GhostMode mode) { return out << toString(mode); }

std::istream& operator>>(std::istream& in, Direction& dir) {
    return detail::readEnum<Direction>(
        in, dir, {Direction::None, Direction::Up, Direction::Down, Direction::Left, Direction::Right});
}

std::istream& operator>>(std::istream& in, GhostType& type) {
    return detail::readEnum<GhostType>(
        in, type, {GhostType::Blinky, GhostType::Pinky, GhostType::Inky, GhostType::Clyde});
}

std::istream& operator>>(std::istream& in, GhostMode& mode) {
    return detail::readEnum<GhostMode>(
        in, mode, {GhostMode::Chase, GhostMode::Scatter, GhostMode::Frightened, GhostMode::Eaten});
}

}  // namespace pacman
