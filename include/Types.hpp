#pragma once

#include <cstddef>
#include <vector>

namespace pacman {

enum class Direction { None, Up, Down, Left, Right };

enum class CellType { Empty, Wall, Dot, PowerPellet, GhostHouse };

enum class GameState { Menu, Running, Paused, Won, Lost };

enum class GhostType { Blinky, Pinky, Inky, Clyde };

enum class GhostMode { Chase, Scatter, Frightened, Eaten };

enum class Command { None, MoveUp, MoveDown, MoveLeft, MoveRight, Pause, Quit };

struct Position {
    int x{0};
    int y{0};

    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Position& other) const { return !(*this == other); }
};

struct Pacman {
    Position position{};
    Direction direction{Direction::None};
    Direction nextDirection{Direction::None};  // direcția cerută de jucător
    int lives{3};
};

struct Ghost {
    GhostType type{GhostType::Blinky};
    GhostMode mode{GhostMode::Scatter};
    Position position{};
    Position spawn{};
    Direction direction{Direction::None};
    int frightenedTicks{0};  // tick-uri rămase în modul Frightened
};

struct Board {
    int width{0};
    int height{0};
    std::vector<CellType> cells;  // stocare liniară: index = y * width + x

    bool inBounds(int x, int y) const { return x >= 0 && y >= 0 && x < width && y < height; }

    CellType& at(int x, int y) { return cells[static_cast<std::size_t>(y * width + x)]; }
    const CellType& at(int x, int y) const { return cells[static_cast<std::size_t>(y * width + x)]; }
};

struct Score {
    int current{0};
    int best{0};
};

struct GameConfig {
    int initialLives{3};
    int frightenedDuration{40};  // în tick-uri
    int dotPoints{10};
    int powerPelletPoints{50};
    int ghostPoints{200};
};

}  