#pragma once

#include <cstddef>
#include <iosfwd>
#include <vector>

namespace pacman {

enum class Direction { None, Up, Down, Left, Right };

enum class CellType { Empty, Wall, Dot, PowerPellet, GhostHouse };

enum class GameState { Menu, Running, Paused, Won, Lost };

enum class GhostType { Blinky, Pinky, Inky, Clyde };

enum class GhostMode { Chase, Scatter, Frightened, Eaten };

// Comenzi abstracte, independente de dispozitivul de intrare.
enum class Command { None, MoveUp, MoveDown, MoveLeft, MoveRight, Pause, Quit };

// ======================================================================
//  Structurile de joc. Fiecare are:
//    - constructor implicit, cu parametri și de copiere
//    - operator de atribuire
//    - operatori de comparare
//    - operatori << (ieșire) și >> (intrare), declarați după structuri
// ======================================================================

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

struct Pacman {
    Position position{};
    Position spawn{};
    Direction direction{Direction::None};
    Direction nextDirection{Direction::None};  // direcția cerută de jucător
    int lives{3};

    Pacman();                                          // implicit: 3 vieți, poziția (0, 0)
    explicit Pacman(const Position& spawn, int lives = 3);  // cu parametri
    Pacman(const Pacman& other);                       // de copiere
    Pacman& operator=(const Pacman& other);            // de atribuire

    bool operator==(const Pacman& other) const;
    bool operator!=(const Pacman& other) const;

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

    Ghost();                                         // implicit: Blinky la (0, 0)
    Ghost(GhostType type, const Position& spawn);    // cu parametri
    Ghost(const Ghost& other);                       // de copiere
    Ghost& operator=(const Ghost& other);            // de atribuire

    bool operator==(const Ghost& other) const;
    bool operator!=(const Ghost& other) const;

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
};

struct Score {
    int current{0};
    int best{0};

    Score();                                   // implicit: 0 / 0
    Score(int current, int best);              // cu parametri
    Score(const Score& other);                 // de copiere
    Score& operator=(const Score& other);      // de atribuire

    // Comparare lexicografică: întâi scorul curent, apoi recordul.
    bool operator==(const Score& other) const;
    bool operator!=(const Score& other) const;
    bool operator<(const Score& other) const;
    bool operator>(const Score& other) const;
    bool operator<=(const Score& other) const;
    bool operator>=(const Score& other) const;

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

    GameConfig();                                              // implicit: valorile de mai sus
    GameConfig(int initialLives, int frightenedDuration, int dotPoints,
               int powerPelletPoints, int ghostPoints);        // cu parametri (restul rămân implicite)
    GameConfig(const GameConfig& other);                       // de copiere
    GameConfig& operator=(const GameConfig& other);            // de atribuire

    bool operator==(const GameConfig& other) const;
    bool operator!=(const GameConfig& other) const;

    bool isValid() const;
};

// ======================================================================
//  Funcții ajutătoare
// ======================================================================

Direction opposite(Direction dir);
Position directionOffset(Direction dir);  // (dx, dy) pentru direcție
Direction directionFromCommand(Command command);

const char* toString(Direction dir);
const char* toString(GameState state);
const char* toString(GhostType type);
const char* toString(GhostMode mode);

// Reprezentarea unei celule ca un caracter ('#', '.', 'o', '-', ' ') și invers.
char toChar(CellType cell);
bool cellFromChar(char symbol, CellType& cell);

// ======================================================================
//  Operatori de ieșire (<<) și de intrare (>>)
//  Operatorii >> modifică obiectul doar dacă citirea reușește complet;
//  la format greșit setează failbit și lasă obiectul neschimbat.
// ======================================================================

// Enum-uri: se scriu și se citesc ca nume (ex. "Left"); CellType/GameState doar la ieșire.
std::ostream& operator<<(std::ostream& out, Direction dir);
std::istream& operator>>(std::istream& in, Direction& dir);
std::ostream& operator<<(std::ostream& out, CellType cell);
std::ostream& operator<<(std::ostream& out, GameState state);
std::ostream& operator<<(std::ostream& out, GhostType type);
std::istream& operator>>(std::istream& in, GhostType& type);
std::ostream& operator<<(std::ostream& out, GhostMode mode);
std::istream& operator>>(std::istream& in, GhostMode& mode);

// Structuri:
//   Position   ->  (3,4)
//   Pacman     ->  Pacman pos=(1,2) spawn=(1,2) dir=Left next=Up lives=3
//   Ghost      ->  Ghost type=Blinky mode=Scatter pos=(1,2) spawn=(1,2) dir=None frightened=0
//   Board      ->  "Board 5x3", apoi 3 rânduri de câte 5 caractere
//   Score      ->  Score current=10 best=20
//   GameConfig ->  GameConfig lives=3 frightened=40 dot=10 pellet=50 ghost=200 scatter=30 chase=80 divider=2 tick=120
std::ostream& operator<<(std::ostream& out, const Position& position);
std::istream& operator>>(std::istream& in, Position& position);
std::ostream& operator<<(std::ostream& out, const Pacman& pacman);
std::istream& operator>>(std::istream& in, Pacman& pacman);
std::ostream& operator<<(std::ostream& out, const Ghost& ghost);
std::istream& operator>>(std::istream& in, Ghost& ghost);
std::ostream& operator<<(std::ostream& out, const Board& board);
std::istream& operator>>(std::istream& in, Board& board);
std::ostream& operator<<(std::ostream& out, const Score& score);
std::istream& operator>>(std::istream& in, Score& score);
std::ostream& operator<<(std::ostream& out, const GameConfig& config);
std::istream& operator>>(std::istream& in, GameConfig& config);

}  // namespace pacman
