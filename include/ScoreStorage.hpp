#pragma once

#include <string>

#include "HighScores.hpp"

namespace pacman {

// Interfață abstractă pentru păstrarea tabelei de scoruri.
// Joculul depinde doar de ea (prin std::unique_ptr<ScoreStorage>), nu de un format anume:
//   - TextScoreStorage    -> fișier text            (biblioteca pacman_highscores)
//   - SqliteScoreStorage  -> bază de date SQLite    (biblioteca pacman_sqlite + SQLite3 externă)
class ScoreStorage {
public:
    virtual ~ScoreStorage() = default;

    // Citește tabela. Returnează false dacă sursa nu există sau nu poate fi citită.
    virtual bool load(HighScores& scores) = 0;
    // Scrie tabela. Returnează false dacă scrierea a eșuat.
    virtual bool save(const HighScores& scores) = 0;
    // Descriere pentru afișare (ex. "text: highscores.txt").
    virtual std::string description() const = 0;
};

// Păstrează scorurile într-un fișier text (un rând pe intrare: "Ana 1200").
class TextScoreStorage : public ScoreStorage {
public:
    explicit TextScoreStorage(std::string path);

    bool load(HighScores& scores) override;
    bool save(const HighScores& scores) override;
    std::string description() const override;

private:
    std::string path_;
};

}  // namespace pacman
