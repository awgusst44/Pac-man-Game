#pragma once

#include <iosfwd>
#include <memory>
#include <string>
#include <vector>

#include "GameEngine.hpp"
#include "HighScores.hpp"
#include "Listener.hpp"
#include "Renderer.hpp"

namespace pacman {

// Aplicația: leagă motorul, desenatorul și ascultătorul și ține tabela de scoruri.
//   - Renderer și Listener sunt deținute prin std::unique_ptr (proprietate unică, polimorfism);
//   - configurația este partajată cu motorul prin std::shared_ptr;
//   - restul se transmite prin referințe.
class Game {
public:
    Game(std::unique_ptr<Renderer> renderer, std::unique_ptr<Listener> listener,
         std::shared_ptr<const GameConfig> config, const std::string& playerName);

    // Nivelul implicit, în cazul în care nu se poate citi un fișier.
    static std::vector<std::string> defaultLevel();

    bool loadLevel(const std::vector<std::string>& layout);
    bool loadLevel(std::istream& in);

    // Fișierul tabelei de scoruri: se citește acum și se rescrie când scorul intră în top.
    bool setScoresFile(const std::string& path);

    void run();      // bucla jocului, până la Q
    void step();     // un singur tick: intrare, simulare, desenare, scoruri
    void finish();   // înregistrează scorul unui joc întrerupt

    const GameEngine& engine() const { return engine_; }
    const HighScores& highScores() const { return scores_; }
    const std::string& playerName() const { return playerName_; }

private:
    void trackResult();
    void recordScore();

    std::unique_ptr<Renderer> renderer_;
    std::unique_ptr<Listener> listener_;
    std::shared_ptr<const GameConfig> config_;
    GameEngine engine_;
    HighScores scores_;
    std::string playerName_;
    std::string scoresPath_;
    bool recorded_{false};  // scorul jocului curent a fost deja înregistrat
};

}  // namespace pacman
