#pragma once

#include <iosfwd>
#include <memory>
#include <random>
#include <string>
#include <vector>

#include "Types.hpp"

namespace pacman {

// Motorul de joc: logica jocului, fără dependență de desenare sau intrare.
class GameEngine {
public:
    // Configurația se copiază într-un obiect partajat (shared_ptr)...
    explicit GameEngine(const GameConfig& config = GameConfig{});
    // ...sau se partajează cu alte obiecte (de exemplu cu Game): același obiect, fără copii.
    explicit GameEngine(std::shared_ptr<const GameConfig> config);

    // Încarcă nivelul dintr-un șablon text:
    // '#' perete, '.' punct, 'o' bilă de putere, 'P' Pacman,
    // 'G' fantomă (în casa fantomelor), '-' casa fantomelor, ' ' gol.
    // Returnează false (și nu modifică jocul) dacă șablonul este invalid sau
    // dacă nu toate punctele pot fi mâncate de Pacman.
    bool loadLevel(const std::vector<std::string>& layout);

    // Citește nivelul dintr-un flux (fișier, std::stringstream...).
    // Rândurile care încep cu ';' sunt comentarii; rândurile mai scurte se completează cu spații.
    bool loadLevel(std::istream& in);

    void start();   // Menu -> Running
    void reset();   // reîncarcă nivelul curent

    // Primește o comandă de la Listener.
    void handleCommand(Command command);

    // Un pas de simulare (apelat periodic).
    void update();

    // Acces doar în citire, folosit de Renderer.
    GameState state() const { return state_; }
    const GameConfig& config() const { return *config_; }
    const Board& board() const { return board_; }
    const Pacman& pacman() const { return pacman_; }
    const std::vector<Ghost>& ghosts() const { return ghosts_; }
    const Score& score() const { return score_; }
    int dotsRemaining() const { return dotsRemaining_; }
    bool isRunning() const { return state_ == GameState::Running; }
    bool quitRequested() const { return quit_; }

private:
    void movePacman();
    void moveGhosts();
    void updateGhostModes();
    void checkCollisions();
    void eatAt(const Position& position);
    void loseLife();

    bool canMove(const Position& from, Direction dir) const;          // pentru Pacman
    bool canGhostMove(const Ghost& ghost, Direction dir) const;
    Position step(const Position& from, Direction dir) const;         // include tunelul

    Direction chooseDirection(const Ghost& ghost);
    Direction bfsDirection(const Ghost& ghost, const Position& target) const;
    Position ghostTarget(const Ghost& ghost) const;
    Position scatterCorner(GhostType type) const;

    std::shared_ptr<const GameConfig> config_;
    GameState state_{GameState::Menu};
    Board board_;
    Pacman pacman_;
    std::vector<Ghost> ghosts_;
    Score score_;
    std::vector<std::string> layout_;  // pentru reset()
    int dotsRemaining_{0};
    int tick_{0};
    bool quit_{false};
    std::mt19937 rng_;
};

// Scrie starea curentă în formatul nivelului ('P' Pacman, 'G' fantome, restul simbolurile celulelor).
std::ostream& operator<<(std::ostream& out, const GameEngine& engine);

}  // namespace pacman
