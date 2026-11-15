#include <algorithm>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "Demo.hpp"
#include "Game.hpp"
#include "HighScores.hpp"
#include "Listener.hpp"
#include "Renderer.hpp"
#include "ScoreStorage.hpp"
#include "SqliteScoreStorage.hpp"

namespace {

using Args = std::vector<std::string>;

bool hasFlag(const Args& args, const std::string& flag) {
    return std::find(args.begin(), args.end(), flag) != args.end();
}

// Valoarea de după o opțiune (ex. "--level fisier.txt"), sau valoarea implicită.
std::string optionValue(const Args& args, const std::string& option, const std::string& fallback) {
    const auto found = std::find(args.begin(), args.end(), option);
    if (found == args.end() || std::next(found) == args.end()) {
        return fallback;
    }
    return *std::next(found);
}

bool endsWith(const std::string& text, const std::string& suffix) {
    return text.size() >= suffix.size() &&
           std::equal(suffix.rbegin(), suffix.rend(), text.rbegin());
}

// Alege locul de păstrare a scorurilor după extensia fișierului:
// .db / .sqlite -> bază de date SQLite, orice altceva -> fișier text.
std::unique_ptr<pacman::ScoreStorage> makeStorage(const std::string& path) {
    if (endsWith(path, ".db") || endsWith(path, ".sqlite")) {
        return std::make_unique<pacman::SqliteScoreStorage>(path);
    }
    return std::make_unique<pacman::TextScoreStorage>(path);
}

void printHelp() {
    std::cout << "Pacman\n"
                 "  pacman                    porneste jocul\n"
                 "  pacman --level <fisier>   incarca nivelul din fisier (implicit levels/level1.txt)\n"
                 "  pacman --name <nume>      numele jucatorului in tabela de scoruri\n"
                 "  pacman --scores           afiseaza tabela de scoruri si iese\n"
                 "  pacman --scores-file <f>  unde se pastreaza scorurile: *.db = SQLite, altfel text\n"
                 "                            (implicit highscores.db)\n"
                 "  pacman --demo             demonstratie (constructori, STL, fluxuri, smart pointers)\n"
                 "  pacman --help             acest mesaj\n";
}

}  // namespace

int main(int argc, char** argv) {
    using namespace pacman;

    const Args args(argv + 1, argv + argc);
    const std::string scoresPath = optionValue(args, "--scores-file", "highscores.db");

    if (hasFlag(args, "--help")) {
        printHelp();
        return 0;
    }
    if (hasFlag(args, "--demo")) {
        return runDemo(std::cout);
    }
    if (hasFlag(args, "--scores")) {
        HighScores table;
        const std::unique_ptr<ScoreStorage> storage = makeStorage(scoresPath);
        storage->load(table);
        std::cout << "Sursa: " << storage->description() << "\n";
        table.print(std::cout);
        return 0;
    }

    auto config = std::make_shared<const GameConfig>();
    Game game(std::make_unique<ConsoleRenderer>(), std::make_unique<ConsoleListener>(), config,
              optionValue(args, "--name", "Player"));

    // Nivelul: din fișier (flux de intrare), altfel cel încorporat.
    const bool levelRequested = hasFlag(args, "--level");
    const std::string levelPath = optionValue(args, "--level", "levels/level1.txt");
    std::ifstream levelFile(levelPath);
    bool loaded = false;
    if (levelFile) {
        loaded = game.loadLevel(levelFile);
        if (!loaded) {
            std::cerr << "Nivelul din '" << levelPath << "' este invalid.\n";
            return 1;
        }
    } else if (levelRequested) {
        std::cerr << "Nu pot deschide nivelul '" << levelPath << "'.\n";
        return 1;
    } else {
        loaded = game.loadLevel(Game::defaultLevel());
    }
    if (!loaded) {
        std::cerr << "Nivelul nu a putut fi incarcat.\n";
        return 1;
    }

    game.setStorage(makeStorage(scoresPath));
    game.run();
    game.highScores().print(std::cout);
    return 0;
}
