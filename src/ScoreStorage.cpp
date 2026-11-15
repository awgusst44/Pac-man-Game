// Biblioteca pacman_highscores (partea a doua): păstrarea scorurilor într-un fișier text.
#include "ScoreStorage.hpp"

#include <utility>

namespace pacman {

TextScoreStorage::TextScoreStorage(std::string path) : path_(std::move(path)) {}

bool TextScoreStorage::load(HighScores& scores) { return scores.loadFromFile(path_); }

bool TextScoreStorage::save(const HighScores& scores) { return scores.saveToFile(path_); }

std::string TextScoreStorage::description() const { return "text: " + path_; }

}  // namespace pacman
