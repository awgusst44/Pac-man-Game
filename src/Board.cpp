// Biblioteca pacman_board: clasa Board (grila labirintului).
#include "Board.hpp"

#include <algorithm>
#include <array>
#include <istream>
#include <iterator>
#include <limits>
#include <ostream>
#include <queue>
#include <string>

#include "StreamParsing.hpp"

namespace pacman {

namespace {
constexpr int kMaxBoardSide = 1000;  // limită de siguranță la citirea unei grile
}  // namespace

Board::Board() : width(0), height(0), cells() {}

Board::Board(int w, int h, CellType fill) : width(std::max(w, 0)), height(std::max(h, 0)), cells() {
    cells.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
    std::fill(cells.begin(), cells.end(), fill);  // umple grila
}

Board::Board(const Board& other) : width(other.width), height(other.height), cells() {
    cells.reserve(other.cells.size());
    std::copy(other.cells.begin(), other.cells.end(), std::back_inserter(cells));  // copie profundă
}

Board& Board::operator=(const Board& other) {
    if (this != &other) {
        width = other.width;
        height = other.height;
        cells.clear();
        cells.reserve(other.cells.size());
        std::copy(other.cells.begin(), other.cells.end(), std::back_inserter(cells));
    }
    return *this;
}

bool Board::operator==(const Board& other) const {
    return width == other.width && height == other.height &&
           std::equal(cells.begin(), cells.end(), other.cells.begin(), other.cells.end());
}
bool Board::operator!=(const Board& other) const { return !(*this == other); }

CellType Board::get(int x, int y) const { return inBounds(x, y) ? at(x, y) : CellType::Wall; }

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

std::set<Position> Board::positionsOf(CellType type) const {
    std::set<Position> result;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (at(x, y) == type) {
                result.insert(Position(x, y));
            }
        }
    }
    return result;
}

std::set<Position> Board::reachableFrom(const Position& start) const {
    static const std::array<Direction, 4> directions{
        {Direction::Up, Direction::Left, Direction::Down, Direction::Right}};

    std::set<Position> reached;
    if (!isWalkable(start, false)) {
        return reached;
    }
    std::queue<Position> pending;
    reached.insert(start);
    pending.push(start);
    while (!pending.empty()) {
        const Position current = pending.front();
        pending.pop();
        for (Direction dir : directions) {
            const Position next = wrap(current.moved(dir));
            if (isWalkable(next, false) && reached.insert(next).second) {
                pending.push(next);  // celulă nouă, încă nevizitată
            }
        }
    }
    return reached;
}

std::ostream& operator<<(std::ostream& out, const Board& board) {
    out << "Board " << board.width << 'x' << board.height << '\n';
    for (int y = 0; y < board.height; ++y) {
        for (int x = 0; x < board.width; ++x) {
            out << toChar(board.at(x, y));
        }
        out << '\n';
    }
    return out;
}

std::istream& operator>>(std::istream& in, Board& board) {
    using namespace detail;
    int w = 0;
    int h = 0;
    if (!(expectKeyword(in, "Board") && static_cast<bool>(in >> w) && expectChar(in, 'x') &&
          static_cast<bool>(in >> h))) {
        return in;
    }
    if (w < 0 || h < 0 || w > kMaxBoardSide || h > kMaxBoardSide) {
        in.setstate(std::ios::failbit);
        return in;
    }
    in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // restul liniei de antet

    Board tmp(w, h);
    std::string row;
    for (int y = 0; y < h; ++y) {
        if (!std::getline(in, row)) {
            in.setstate(std::ios::failbit);
            return in;
        }
        if (!row.empty() && row.back() == '\r') {
            row.pop_back();  // fișiere cu sfârșit de linie Windows
        }
        if (static_cast<int>(row.size()) != w) {
            in.setstate(std::ios::failbit);
            return in;
        }
        for (int x = 0; x < w; ++x) {
            if (!cellFromChar(row[static_cast<std::size_t>(x)], tmp.at(x, y))) {
                in.setstate(std::ios::failbit);
                return in;
            }
        }
    }
    board = tmp;
    return in;
}

}  // namespace pacman
