// Biblioteca pacman_position: clasa Position.
#include "Position.hpp"

#include <istream>
#include <ostream>
#include <tuple>

#include "StreamParsing.hpp"

namespace pacman {

Position::Position() : x(0), y(0) {}

Position::Position(int x, int y) : x(x), y(y) {}

Position::Position(const Position& other) : x(other.x), y(other.y) {}

Position& Position::operator=(const Position& other) {
    if (this != &other) {  // protecție la auto-atribuire
        x = other.x;
        y = other.y;
    }
    return *this;
}

bool Position::operator==(const Position& other) const { return x == other.x && y == other.y; }
bool Position::operator!=(const Position& other) const { return !(*this == other); }
bool Position::operator<(const Position& other) const {
    return std::tie(x, y) < std::tie(other.x, other.y);  // comparare lexicografică
}
bool Position::operator>(const Position& other) const { return other < *this; }
bool Position::operator<=(const Position& other) const { return !(other < *this); }
bool Position::operator>=(const Position& other) const { return !(*this < other); }

Position Position::moved(Direction dir) const {
    const Position offset = directionOffset(dir);
    return {x + offset.x, y + offset.y};
}

int Position::distanceSquared(const Position& other) const {
    const int dx = x - other.x;
    const int dy = y - other.y;
    return dx * dx + dy * dy;
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

std::ostream& operator<<(std::ostream& out, const Position& position) {
    return out << '(' << position.x << ',' << position.y << ')';
}

std::istream& operator>>(std::istream& in, Position& position) {
    int x = 0;
    int y = 0;
    if (detail::expectChar(in, '(') && static_cast<bool>(in >> x) && detail::expectChar(in, ',') &&
        static_cast<bool>(in >> y) && detail::expectChar(in, ')')) {
        position = Position(x, y);
    }
    return in;
}

}  // namespace pacman
