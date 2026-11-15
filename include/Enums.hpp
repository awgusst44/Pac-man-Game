#pragma once

#include <iosfwd>
#include <string>

namespace pacman {

enum class Direction { None, Up, Down, Left, Right };

enum class CellType { Empty, Wall, Dot, PowerPellet, GhostHouse };

enum class GameState { Menu, Running, Paused, Won, Lost };

enum class GhostType { Blinky, Pinky, Inky, Clyde };

enum class GhostMode { Chase, Scatter, Frightened, Eaten };

// Comenzi abstracte, independente de dispozitivul de intrare.
enum class Command { None, MoveUp, MoveDown, MoveLeft, MoveRight, Pause, Quit };

// Funcții ajutătoare (biblioteca pacman_enums).
Direction opposite(Direction dir);
Direction directionFromCommand(Command command);

std::string toString(Direction dir);
std::string toString(GameState state);
std::string toString(GhostType type);
std::string toString(GhostMode mode);

// Reprezentarea unei celule ca un caracter ('#', '.', 'o', '-', ' ') și invers.
char toChar(CellType cell);
bool cellFromChar(char symbol, CellType& cell);

// Enum-uri: se scriu și se citesc ca nume (ex. "Left"); CellType/GameState doar la ieșire.
std::ostream& operator<<(std::ostream& out, Direction dir);
std::istream& operator>>(std::istream& in, Direction& dir);
std::ostream& operator<<(std::ostream& out, CellType cell);
std::ostream& operator<<(std::ostream& out, GameState state);
std::ostream& operator<<(std::ostream& out, GhostType type);
std::istream& operator>>(std::istream& in, GhostType& type);
std::ostream& operator<<(std::ostream& out, GhostMode mode);
std::istream& operator>>(std::istream& in, GhostMode& mode);

}  // namespace pacman
