#include "Renderer.hpp"

#include <iostream>
#include <sstream>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace pacman {

namespace {

// Coduri ANSI pentru culori (funcționează în Windows Terminal / PowerShell / Linux).
constexpr const char* kReset = "\x1b[0m";
constexpr const char* kWall = "\x1b[34;1m";
constexpr const char* kDot = "\x1b[37m";
constexpr const char* kPellet = "\x1b[97;1m";
constexpr const char* kHouse = "\x1b[90m";
constexpr const char* kPacman = "\x1b[93;1m";
constexpr const char* kFrightened = "\x1b[97;44m";
constexpr const char* kEaten = "\x1b[90m";
constexpr const char* kClearLine = "\x1b[K";

const char* ghostColor(GhostType type) {
    switch (type) {
        case GhostType::Blinky: return "\x1b[91;1m";  // roșu
        case GhostType::Pinky:  return "\x1b[95;1m";  // roz
        case GhostType::Inky:   return "\x1b[96;1m";  // cyan
        case GhostType::Clyde:  return "\x1b[33;1m";  // portocaliu
    }
    return kReset;
}

}  // namespace

void ConsoleRenderer::init() {
#ifdef _WIN32
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (out != INVALID_HANDLE_VALUE && GetConsoleMode(out, &mode)) {
        SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
#endif
    std::cout << "\x1b[?25l";  // ascunde cursorul
    clearScreen();
}

void ConsoleRenderer::clearScreen() {
    std::cout << "\x1b[2J\x1b[H" << std::flush;
}

void ConsoleRenderer::render(const GameEngine& engine) {
    std::ostringstream frame;
    frame << "\x1b[H";  // cursor sus-stânga (fără clear, ca să nu pâlpâie)
    drawHud(engine, frame);

    const Board& board = engine.board();
    for (int y = 0; y < board.height; ++y) {
        for (int x = 0; x < board.width; ++x) {
            frame << symbolFor(engine, x, y);
        }
        frame << kClearLine << '\n';
    }

    drawFooter(engine, frame);
    frame << "\x1b[J";  // șterge restul ecranului
    std::cout << frame.str() << std::flush;
}

void ConsoleRenderer::shutdown() {
    std::cout << kReset << "\x1b[?25h\n" << std::flush;  // arată cursorul
}

std::string ConsoleRenderer::symbolFor(const GameEngine& engine, int x, int y) const {
    const Position here{x, y};
    if (engine.pacman().position == here) {
        return std::string(kPacman) + "C" + kReset;
    }
    for (const Ghost& ghost : engine.ghosts()) {
        if (ghost.position != here) {
            continue;
        }
        switch (ghost.mode) {
            case GhostMode::Frightened: return std::string(kFrightened) + "w" + kReset;
            case GhostMode::Eaten:      return std::string(kEaten) + "e" + kReset;
            default:                    return std::string(ghostColor(ghost.type)) + "G" + kReset;
        }
    }
    switch (engine.board().at(x, y)) {
        case CellType::Wall:        return std::string(kWall) + "#" + kReset;
        case CellType::Dot:         return std::string(kDot) + "." + kReset;
        case CellType::PowerPellet: return std::string(kPellet) + "o" + kReset;
        case CellType::GhostHouse:  return std::string(kHouse) + "-" + kReset;
        case CellType::Empty:       return " ";
    }
    return " ";
}

void ConsoleRenderer::drawHud(const GameEngine& engine, std::ostream& out) const {
    out << "SCOR: " << engine.score().current << "   RECORD: " << engine.score().best
        << "   VIETI: " << engine.pacman().lives << "   PUNCTE RAMASE: " << engine.dotsRemaining()
        << kClearLine << "\n\n";
}

void ConsoleRenderer::drawFooter(const GameEngine& engine, std::ostream& out) const {
    out << '\n';
    switch (engine.state()) {
        case GameState::Menu:
            out << "PACMAN - apasa W/A/S/D sau sagetile pentru a incepe. Q - iesire";
            break;
        case GameState::Running:
            out << "W/A/S/D sau sageti - miscare | P - pauza | Q - iesire";
            break;
        case GameState::Paused:
            out << "PAUZA - apasa P pentru a continua";
            break;
        case GameState::Won:
            out << "AI CASTIGAT! Scor: " << engine.score().current
                << ". Apasa o tasta de miscare pentru a juca din nou sau Q pentru iesire";
            break;
        case GameState::Lost:
            out << "GAME OVER. Scor: " << engine.score().current
                << ". Apasa o tasta de miscare pentru a juca din nou sau Q pentru iesire";
            break;
    }
    out << kClearLine << '\n';
}

}  // namespace pacman
