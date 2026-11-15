#pragma once

#include <cstddef>
#include <iosfwd>
#include <set>
#include <vector>

#include "Enums.hpp"
#include "Position.hpp"

namespace pacman {

struct Board {
    int width{0};
    int height{0};
    std::vector<CellType> cells;  // stocare liniară: index = y * width + x

    Board();                                                  // implicit: grilă goală 0 x 0
    Board(int w, int h, CellType fill = CellType::Empty);     // cu parametri
    Board(const Board& other);                                // de copiere (copie profundă)
    Board& operator=(const Board& other);                     // de atribuire

    bool operator==(const Board& other) const;
    bool operator!=(const Board& other) const;

    bool inBounds(int x, int y) const { return x >= 0 && y >= 0 && x < width && y < height; }

    CellType& at(int x, int y) { return cells[static_cast<std::size_t>(y * width + x)]; }
    const CellType& at(int x, int y) const { return cells[static_cast<std::size_t>(y * width + x)]; }

    // Tipul celulei sau Wall dacă este în afara grilei.
    CellType get(int x, int y) const;
    // Aplică tunelul: ieșirea pe o margine readuce poziția pe marginea opusă.
    Position wrap(const Position& position) const;
    // Poate fi ocupată celula? Peretele nu; casa fantomelor doar dacă allowGhostHouse.
    bool isWalkable(const Position& position, bool allowGhostHouse) const;
    // Numără celulele de un anumit tip.
    int count(CellType type) const;
    // Pozițiile tuturor celulelor de un anumit tip (std::set, ordonate).
    std::set<Position> positionsOf(CellType type) const;
    // Celulele la care poate ajunge Pacman pornind de la start (parcurgere în lățime, cu tunel).
    std::set<Position> reachableFrom(const Position& start) const;
};

// Format:  "Board 5x3", apoi 3 rânduri de câte 5 caractere
std::ostream& operator<<(std::ostream& out, const Board& board);
std::istream& operator>>(std::istream& in, Board& board);

}  // namespace pacman
