#pragma once

#include <cstddef>
#include <vector>

namespace pacman {

enum class Direction { None, Up, Down, Left, Right };

enum class CellType { Empty, Wall, Dot, PowerPellet, GhostHouse };

enum class GameState { Menu, Running, Paused, Won, Lost };

enum class GhostType { Blinky, Pinky, Inky, Clyde };

enum class GhostMode { Chase, Scatter, Frightened, Eaten };

// Comenzi abstracte, independente de dispozitivul de intrare.
enum class Command { None, MoveUp, MoveDown, MoveLeft, MoveRight, Pause, Quit };

struct Position {
    int x{0};
    int y{0};

    bool operator==(const Position& other) const { return x == other.x && y == other.y; }
    bool operator!=(const Position& other) const { return !(*this == other); }

    // Poziția vecină în direcția dată (fără tunel; vezi Board::wrap).
    Position moved(Direction dir) const;
    // Pătratul distanței euclidiene (evită radicalul).
    int distanceSquared(const Position& other) const;
};

// Funcții ajutătoare pentru enum-uri.
Direction opposite(Direction dir);
Position directionOffset(Direction dir);  // (dx, dy) pentru direcție
Direction directionFromCommand(Command command);
const char* toString(Direction dir);
const char* toString(GameState state);
const char* toString(GhostMode mode);

struct Pacman {
    Position position{};
    Position spawn{};
    Direction direction{Direction::None};
    Direction nextDirection{Direction::None};  // direcția cerută de jucător
    int lives{3};

    void reset(int initialLives);  // poziție de start + vieți inițiale
    void respawn();                // revine la start, păstrează viețile
    void requestDirection(Direction dir);
    void loseLife();
    bool isAlive() const;
};

struct Ghost {
    GhostType type{GhostType::Blinky};
    GhostMode mode{GhostMode::Scatter};
    Position position{};
    Position spawn{};
    Direction direction{Direction::None};
    int frightenedTicks{0};  // tick-uri rămase în modul Frightened

    void reset();                 // revine la start
    void frighten(int ticks);     // devine vulnerabilă și își inversează direcția
    void tick();                  // scade timerul de vulnerabilitate
    void markEaten();             // a fost mâncată de Pacman
    bool isVulnerable() const;    // mode == Frightened
    bool isDangerous() const;     // mode == Chase sau Scatter
};

struct Board {
    int width{0};
    int height{0};
    std::vector<CellType> cells;  // stocare liniară: index = y * width + x

    Board() = default;
    Board(int w, int h, CellType fill = CellType::Empty);

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
};

struct Score {
    int current{0};
    int best{0};

    void add(int points);  // adaugă puncte și actualizează recordul
    void reset();          // scor curent 0, recordul se păstrează
};

struct GameConfig {
    int initialLives{3};
    int frightenedDuration{40};  // în tick-uri
    int dotPoints{10};
    int powerPelletPoints{50};
    int ghostPoints{200};
    int scatterTicks{30};        // faza de retragere a fantomelor
    int chaseTicks{80};          // faza de urmărire
    int ghostSpeedDivider{2};    // fantomele se mișcă o dată la N tick-uri
    int tickMillis{120};         // durata unui tick în main

    bool isValid() const;
};

}  // namespace pacman
