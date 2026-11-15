#pragma once

#include <iosfwd>

#include "Enums.hpp"

namespace pacman {

struct Position {
    int x{0};
    int y{0};

    Position();                                   // implicit: (0, 0)
    Position(int x, int y);                       // cu parametri
    Position(const Position& other);              // de copiere
    Position& operator=(const Position& other);   // de atribuire

    // Comparare lexicografică: întâi x, apoi y.
    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;
    bool operator<(const Position& other) const;
    bool operator>(const Position& other) const;
    bool operator<=(const Position& other) const;
    bool operator>=(const Position& other) const;

    // Poziția vecină în direcția dată (fără tunel; vezi Board::wrap).
    Position moved(Direction dir) const;
    // Pătratul distanței euclidiene (evită radicalul).
    int distanceSquared(const Position& other) const;
};

Position directionOffset(Direction dir);  // (dx, dy) pentru direcție

// Format:  (3,4)
std::ostream& operator<<(std::ostream& out, const Position& position);
std::istream& operator>>(std::istream& in, Position& position);

}  // namespace pacman
