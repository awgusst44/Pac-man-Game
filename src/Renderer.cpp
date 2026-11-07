#include "Renderer.hpp"

#include <iostream>
#include <sstream>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

namespace pacman {

// ======================================================================
//  Renderer (clasa abstractă): ordinea desenării
// ======================================================================

void Renderer::render(const GameEngine& engine) {
    beginFrame();
    drawBoard(engine.board());
    for (const Ghost& ghost : engine.ghosts()) {
        drawGhost(ghost);
    }
    drawPacman(engine.pacman());  // ultimul: Pacman apare deasupra fantomelor
    drawHud(engine.score(), engine.pacman().lives, engine.dotsRemaining());
    drawStatus(engine.state(), engine.score());
    endFrame();
}

// ======================================================================
//  ConsoleRenderer
// ======================================================================

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

std::string cellSymbol(CellType cell) {
    switch (cell) {
        case CellType::Wall:        return std::string(kWall) + "#" + kReset;
        case CellType::Dot:         return std::string(kDot) + "." + kReset;
        case CellType::PowerPellet: return std::string(kPellet) + "o" + kReset;
        case CellType::GhostHouse:  return std::string(kHouse) + "-" + kReset;
        case CellType::Empty:       return " ";
    }
    return " ";
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

void ConsoleRenderer::shutdown() {
    std::cout << kReset << "\x1b[?25h\n" << std::flush;  // arată cursorul
}

void ConsoleRenderer::clearScreen() { std::cout << "\x1b[2J\x1b[H" << std::flush; }

void ConsoleRenderer::beginFrame() {
    width_ = 0;
    height_ = 0;
    cells_.clear();
    hud_.clear();
    status_.clear();
}

void ConsoleRenderer::drawBoard(const Board& board) {
    width_ = board.width;
    height_ = board.height;
    cells_.assign(static_cast<std::size_t>(width_) * static_cast<std::size_t>(height_), " ");
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            put(Position(x, y), cellSymbol(board.at(x, y)));
        }
    }
}

void ConsoleRenderer::put(const Position& position, const std::string& symbol) {
    if (position.x < 0 || position.y < 0 || position.x >= width_ || position.y >= height_) {
        return;  // în afara grilei desenate
    }
    cells_[static_cast<std::size_t>(position.y * width_ + position.x)] = symbol;
}

void ConsoleRenderer::drawPacman(const Pacman& pacman) {
    put(pacman.position, std::string(kPacman) + "C" + kReset);
}

void ConsoleRenderer::drawGhost(const Ghost& ghost) {
    switch (ghost.mode) {
        case GhostMode::Frightened:
            put(ghost.position, std::string(kFrightened) + "w" + kReset);
            break;
        case GhostMode::Eaten:
            put(ghost.position, std::string(kEaten) + "e" + kReset);
            break;
        default:
            put(ghost.position, std::string(ghostColor(ghost.type)) + "G" + kReset);
            break;
    }
}

void ConsoleRenderer::drawHud(const Score& score, int lives, int dotsRemaining) {
    std::ostringstream text;
    text << "SCOR: " << score.current << "   RECORD: " << score.best << "   VIETI: " << lives
         << "   PUNCTE RAMASE: " << dotsRemaining;
    hud_ = text.str();
}

void ConsoleRenderer::drawStatus(GameState state, const Score& score) {
    std::ostringstream text;
    switch (state) {
        case GameState::Menu:
            text << "PACMAN - apasa W/A/S/D sau sagetile pentru a incepe. Q - iesire";
            break;
        case GameState::Running:
            text << "W/A/S/D sau sageti - miscare | P - pauza | Q - iesire";
            break;
        case GameState::Paused:
            text << "PAUZA - apasa P pentru a continua";
            break;
        case GameState::Won:
            text << "AI CASTIGAT! Scor: " << score.current
                 << ". Apasa o tasta de miscare pentru a juca din nou sau Q pentru iesire";
            break;
        case GameState::Lost:
            text << "GAME OVER. Scor: " << score.current
                 << ". Apasa o tasta de miscare pentru a juca din nou sau Q pentru iesire";
            break;
    }
    status_ = text.str();
}

void ConsoleRenderer::endFrame() {
    std::ostringstream frame;
    frame << "\x1b[H";  // cursor sus-stânga (fără clear, ca să nu pâlpâie)
    frame << hud_ << kClearLine << "\n\n";
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            frame << cells_[static_cast<std::size_t>(y * width_ + x)];
        }
        frame << kClearLine << '\n';
    }
    frame << '\n' << status_ << kClearLine << '\n';
    frame << "\x1b[J";  // șterge restul ecranului
    std::cout << frame.str() << std::flush;
}

}  // namespace pacman
