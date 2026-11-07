#include "Game.hpp"

#include <chrono>
#include <istream>
#include <stdexcept>
#include <thread>
#include <utility>

namespace pacman {

Game::Game(std::unique_ptr<Renderer> renderer, std::unique_ptr<Listener> listener,
           std::shared_ptr<const GameConfig> config, const std::string& playerName)
    : renderer_(std::move(renderer)),
      listener_(std::move(listener)),
      config_(config ? std::move(config) : std::make_shared<const GameConfig>()),
      engine_(config_),
      scores_(),
      playerName_(HighScores::sanitizeName(playerName)) {
    if (!renderer_ || !listener_) {
        throw std::invalid_argument("Game necesita un Renderer si un Listener");
    }
}

std::vector<std::string> Game::defaultLevel() {
    // '#' perete, '.' punct, 'o' bilă de putere, 'P' Pacman, 'G' fantomă, ' ' gol
    return {
        "#####################",
        "#o........#........o#",
        "#.###.###.#.###.###.#",
        "#...................#",
        "#.###.#.#####.#.###.#",
        "#.....#...#...#.....#",
        "#####.### # ###.#####",
        "     .  #GGG#  .     ",
        "#####.# ##### #.#####",
        "#.....#...#...#.....#",
        "#.###.###.#.###.###.#",
        "#o..#.....P.....#..o#",
        "#.###.#.#####.#.###.#",
        "#...................#",
        "#####################",
    };
}

bool Game::loadLevel(const std::vector<std::string>& layout) { return engine_.loadLevel(layout); }

bool Game::loadLevel(std::istream& in) { return engine_.loadLevel(in); }

bool Game::setScoresFile(const std::string& path) {
    scoresPath_ = path;
    return scores_.loadFromFile(path);  // false dacă fișierul nu există încă
}

void Game::step() {
    engine_.handleCommand(listener_->poll());
    engine_.update();
    renderer_->render(engine_);
    trackResult();
}

void Game::run() {
    renderer_->init();
    while (!engine_.quitRequested()) {
        step();
        std::this_thread::sleep_for(std::chrono::milliseconds(config_->tickMillis));
    }
    renderer_->shutdown();
    finish();
}

void Game::finish() {
    if (!recorded_ && engine_.score().current > 0) {
        recordScore();
    }
}

void Game::trackResult() {
    const GameState state = engine_.state();
    if (state == GameState::Running) {
        recorded_ = false;  // a început un joc nou
    } else if ((state == GameState::Won || state == GameState::Lost) && !recorded_) {
        recordScore();
    }
}

void Game::recordScore() {
    recorded_ = true;
    if (scores_.add(playerName_, engine_.score().current) && !scoresPath_.empty()) {
        scores_.saveToFile(scoresPath_);
    }
}

}  // namespace pacman
