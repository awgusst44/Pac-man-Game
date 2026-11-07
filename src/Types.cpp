#include "Types.hpp"

#include <algorithm>
#include <array>
#include <cctype>
#include <initializer_list>
#include <istream>
#include <iterator>
#include <limits>
#include <ostream>
#include <queue>
#include <string>
#include <tuple>

namespace pacman {

// ======================================================================
//  Funcții ajutătoare pentru enum-uri
// ======================================================================

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

// ======================================================================
//  Position
// ======================================================================

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

// ======================================================================
//  Pacman
// ======================================================================

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

// ======================================================================
//  Ghost
// ======================================================================

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

// ======================================================================
//  Board
// ======================================================================

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

// ======================================================================
//  Score
// ======================================================================

Score::Score() : current(0), best(0) {}

Score::Score(int current, int best) : current(current), best(std::max(best, current)) {}

Score::Score(const Score& other) : current(other.current), best(other.best) {}

Score& Score::operator=(const Score& other) {
    if (this != &other) {
        current = other.current;
        best = other.best;
    }
    return *this;
}

bool Score::operator==(const Score& other) const {
    return current == other.current && best == other.best;
}
bool Score::operator!=(const Score& other) const { return !(*this == other); }
bool Score::operator<(const Score& other) const {
    return std::tie(current, best) < std::tie(other.current, other.best);
}
bool Score::operator>(const Score& other) const { return other < *this; }
bool Score::operator<=(const Score& other) const { return !(other < *this); }
bool Score::operator>=(const Score& other) const { return !(*this < other); }

void Score::add(int points) {
    current += points;
    best = std::max(best, current);
}

void Score::reset() { current = 0; }

// ======================================================================
//  GameConfig
// ======================================================================

GameConfig::GameConfig()
    : initialLives(3),
      frightenedDuration(40),
      dotPoints(10),
      powerPelletPoints(50),
      ghostPoints(200),
      scatterTicks(30),
      chaseTicks(80),
      ghostSpeedDivider(2),
      tickMillis(120) {}

GameConfig::GameConfig(int initialLives, int frightenedDuration, int dotPoints,
                       int powerPelletPoints, int ghostPoints)
    : initialLives(initialLives),
      frightenedDuration(frightenedDuration),
      dotPoints(dotPoints),
      powerPelletPoints(powerPelletPoints),
      ghostPoints(ghostPoints),
      scatterTicks(30),
      chaseTicks(80),
      ghostSpeedDivider(2),
      tickMillis(120) {}

GameConfig::GameConfig(const GameConfig& other)
    : initialLives(other.initialLives),
      frightenedDuration(other.frightenedDuration),
      dotPoints(other.dotPoints),
      powerPelletPoints(other.powerPelletPoints),
      ghostPoints(other.ghostPoints),
      scatterTicks(other.scatterTicks),
      chaseTicks(other.chaseTicks),
      ghostSpeedDivider(other.ghostSpeedDivider),
      tickMillis(other.tickMillis) {}

GameConfig& GameConfig::operator=(const GameConfig& other) {
    if (this != &other) {
        initialLives = other.initialLives;
        frightenedDuration = other.frightenedDuration;
        dotPoints = other.dotPoints;
        powerPelletPoints = other.powerPelletPoints;
        ghostPoints = other.ghostPoints;
        scatterTicks = other.scatterTicks;
        chaseTicks = other.chaseTicks;
        ghostSpeedDivider = other.ghostSpeedDivider;
        tickMillis = other.tickMillis;
    }
    return *this;
}

bool GameConfig::operator==(const GameConfig& other) const {
    return initialLives == other.initialLives && frightenedDuration == other.frightenedDuration &&
           dotPoints == other.dotPoints && powerPelletPoints == other.powerPelletPoints &&
           ghostPoints == other.ghostPoints && scatterTicks == other.scatterTicks &&
           chaseTicks == other.chaseTicks && ghostSpeedDivider == other.ghostSpeedDivider &&
           tickMillis == other.tickMillis;
}
bool GameConfig::operator!=(const GameConfig& other) const { return !(*this == other); }

bool GameConfig::isValid() const {
    return initialLives > 0 && frightenedDuration > 0 && dotPoints >= 0 && powerPelletPoints >= 0 &&
           ghostPoints >= 0 && scatterTicks > 0 && chaseTicks > 0 && ghostSpeedDivider > 0 &&
           tickMillis > 0;
}

// ======================================================================
//  Ajutoare pentru operatorii de intrare
// ======================================================================

namespace {

constexpr int kMaxBoardSide = 1000;  // limită de siguranță la citirea unei grile

// Citește un cuvânt format doar din litere (după spații).
bool readWord(std::istream& in, std::string& word) {
    in >> std::ws;
    word.clear();
    while (in && std::isalpha(in.peek())) {
        word.push_back(static_cast<char>(in.get()));
    }
    return !word.empty();
}

// Așteaptă un anumit caracter (după spații); altfel setează failbit.
bool expectChar(std::istream& in, char expected) {
    in >> std::ws;
    if (in.peek() == std::char_traits<char>::to_int_type(expected)) {
        in.get();
        return true;
    }
    in.setstate(std::ios::failbit);
    return false;
}

// Așteaptă un cuvânt-cheie anume; altfel setează failbit.
bool expectKeyword(std::istream& in, const std::string& keyword) {
    std::string word;
    if (readWord(in, word) && word == keyword) {
        return true;
    }
    in.setstate(std::ios::failbit);
    return false;
}

// "nume=" (cuvânt-cheie urmat de '=').
bool field(std::istream& in, const std::string& name) {
    return expectKeyword(in, name) && expectChar(in, '=');
}

bool intField(std::istream& in, const std::string& name, int& value) {
    return field(in, name) && static_cast<bool>(in >> value);
}

// Citește un cuvânt și îl potrivește cu numele valorilor enum.
template <typename E>
std::istream& readEnum(std::istream& in, E& out, std::initializer_list<E> values) {
    std::string word;
    if (!readWord(in, word)) {
        in.setstate(std::ios::failbit);
        return in;
    }
    const auto found = std::find_if(values.begin(), values.end(),
                                    [&word](E value) { return toString(value) == word; });
    if (found == values.end()) {
        in.setstate(std::ios::failbit);
        return in;
    }
    out = *found;
    return in;
}

}  // namespace

// ======================================================================
//  Operatori << și >> pentru enum-uri
// ======================================================================

std::ostream& operator<<(std::ostream& out, Direction dir) { return out << toString(dir); }
std::ostream& operator<<(std::ostream& out, CellType cell) { return out << toChar(cell); }
std::ostream& operator<<(std::ostream& out, GameState state) { return out << toString(state); }
std::ostream& operator<<(std::ostream& out, GhostType type) { return out << toString(type); }
std::ostream& operator<<(std::ostream& out, GhostMode mode) { return out << toString(mode); }

std::istream& operator>>(std::istream& in, Direction& dir) {
    return readEnum<Direction>(
        in, dir, {Direction::None, Direction::Up, Direction::Down, Direction::Left, Direction::Right});
}

std::istream& operator>>(std::istream& in, GhostType& type) {
    return readEnum<GhostType>(
        in, type, {GhostType::Blinky, GhostType::Pinky, GhostType::Inky, GhostType::Clyde});
}

std::istream& operator>>(std::istream& in, GhostMode& mode) {
    return readEnum<GhostMode>(
        in, mode,
        {GhostMode::Chase, GhostMode::Scatter, GhostMode::Frightened, GhostMode::Eaten});
}

// ======================================================================
//  Operatori << și >> pentru structuri
// ======================================================================

// ---------- Position: (x,y) ----------

std::ostream& operator<<(std::ostream& out, const Position& position) {
    return out << '(' << position.x << ',' << position.y << ')';
}

std::istream& operator>>(std::istream& in, Position& position) {
    int x = 0;
    int y = 0;
    if (expectChar(in, '(') && static_cast<bool>(in >> x) && expectChar(in, ',') &&
        static_cast<bool>(in >> y) && expectChar(in, ')')) {
        position = Position(x, y);
    }
    return in;
}

// ---------- Pacman ----------

std::ostream& operator<<(std::ostream& out, const Pacman& pacman) {
    return out << "Pacman pos=" << pacman.position << " spawn=" << pacman.spawn
               << " dir=" << pacman.direction << " next=" << pacman.nextDirection
               << " lives=" << pacman.lives;
}

std::istream& operator>>(std::istream& in, Pacman& pacman) {
    Pacman tmp;
    if (expectKeyword(in, "Pacman") && field(in, "pos") && static_cast<bool>(in >> tmp.position) &&
        field(in, "spawn") && static_cast<bool>(in >> tmp.spawn) && field(in, "dir") &&
        static_cast<bool>(in >> tmp.direction) && field(in, "next") &&
        static_cast<bool>(in >> tmp.nextDirection) && intField(in, "lives", tmp.lives)) {
        pacman = tmp;
    }
    return in;
}

// ---------- Ghost ----------

std::ostream& operator<<(std::ostream& out, const Ghost& ghost) {
    return out << "Ghost type=" << ghost.type << " mode=" << ghost.mode << " pos=" << ghost.position
               << " spawn=" << ghost.spawn << " dir=" << ghost.direction
               << " frightened=" << ghost.frightenedTicks;
}

std::istream& operator>>(std::istream& in, Ghost& ghost) {
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

// ---------- Board: "Board WxH" + H rânduri ----------

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

// ---------- Score ----------

std::ostream& operator<<(std::ostream& out, const Score& score) {
    return out << "Score current=" << score.current << " best=" << score.best;
}

std::istream& operator>>(std::istream& in, Score& score) {
    Score tmp;
    if (expectKeyword(in, "Score") && intField(in, "current", tmp.current) &&
        intField(in, "best", tmp.best)) {
        score = tmp;
    }
    return in;
}

// ---------- GameConfig ----------

std::ostream& operator<<(std::ostream& out, const GameConfig& config) {
    return out << "GameConfig lives=" << config.initialLives
               << " frightened=" << config.frightenedDuration << " dot=" << config.dotPoints
               << " pellet=" << config.powerPelletPoints << " ghost=" << config.ghostPoints
               << " scatter=" << config.scatterTicks << " chase=" << config.chaseTicks
               << " divider=" << config.ghostSpeedDivider << " tick=" << config.tickMillis;
}

std::istream& operator>>(std::istream& in, GameConfig& config) {
    GameConfig tmp;
    if (expectKeyword(in, "GameConfig") && intField(in, "lives", tmp.initialLives) &&
        intField(in, "frightened", tmp.frightenedDuration) && intField(in, "dot", tmp.dotPoints) &&
        intField(in, "pellet", tmp.powerPelletPoints) && intField(in, "ghost", tmp.ghostPoints) &&
        intField(in, "scatter", tmp.scatterTicks) && intField(in, "chase", tmp.chaseTicks) &&
        intField(in, "divider", tmp.ghostSpeedDivider) && intField(in, "tick", tmp.tickMillis)) {
        config = tmp;
    }
    return in;
}

}  // namespace pacman
