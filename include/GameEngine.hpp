#pragma once

#include <string>
#include <vector>

#include "Types.hpp"

namespace pacman {

class GameEngine {
public:
    explicit GameEngine(const GameConfig& config = GameConfig{});

    
    bool loadLevel(const std::vector<std::string>& layout);

    void start();
    void reset();

    void handleCommand(Command command);

    void update();

    GameState state() const { return state_; }
    const Board& board() const { return board_; }
    const Pacman& pacman() const { return pacman_; }
    const std::vector<Ghost>& ghosts() const { return ghosts_; }
    const Score& score() const { return score_; }
    int dotsRemaining() const { return dotsRemaining_; }
    bool isRunning() const { return state_ == GameState::Running; }

private:
    void movePacman();
    void moveGhosts();
    void updateGhostModes();
    void checkCollisions();
    void eatAt(const Position& position);
    bool canMove(const Position& from, Direction dir) const;
    Position step(const Position& from, Direction dir) const;  // include tunelul
    void loseLife();

    GameConfig config_;
    GameState state_{GameState::Menu};
    Board board_;
    Pacman pacman_;
    std::vector<Ghost> ghosts_;
    Score score_;
    int dotsRemaining_{0};
    int tick_{0};
};

}  