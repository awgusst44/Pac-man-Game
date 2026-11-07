#include "Demo.hpp"

#include <algorithm>
#include <array>
#include <iterator>
#include <map>
#include <memory>
#include <numeric>
#include <ostream>
#include <queue>
#include <set>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "Game.hpp"
#include "GameEngine.hpp"
#include "HighScores.hpp"
#include "Renderer.hpp"
#include "Types.hpp"

namespace pacman {

namespace {

void title(std::ostream& out, const std::string& text) { out << "\n=== " << text << " ===\n"; }

}  // namespace

int runDemo(std::ostream& out) {
    out << std::boolalpha;

    // ---------------------------------------------------------------- 1
    title(out, "1. Constructori, atribuire, comparare (lab03)");
    Position a;
    Position b(3, 4);
    Position c(b);
    a = b;
    out << "a = " << a << "  b = " << b << "  c = " << c << "  (a == c: " << (a == c) << ")\n";
    Board board(4, 2, CellType::Dot);
    Board boardCopy(board);
    boardCopy.at(0, 0) = CellType::Wall;
    out << "copia Board este independenta: board == boardCopy -> " << (board == boardCopy) << "\n";

    // ---------------------------------------------------------------- 2
    title(out, "2. Containere STL");
    const std::array<Direction, 4> directions{
        {Direction::Up, Direction::Left, Direction::Down, Direction::Right}};
    out << "std::array<Direction,4>: ";
    std::copy(directions.begin(), directions.end(), std::ostream_iterator<Direction>(out, " "));
    out << "\n";

    GameEngine engine;
    engine.loadLevel(Game::defaultLevel());
    out << "std::vector<Ghost>: " << engine.ghosts().size() << " fantome\n";

    const std::set<Position> food = engine.board().positionsOf(CellType::Dot);
    out << "std::set<Position>: " << food.size() << " puncte, primul " << *food.begin() << "\n";

    const std::set<Position> reachable = engine.board().reachableFrom(engine.pacman().position);
    out << "std::queue + std::set (parcurgere in latime): Pacman ajunge in " << reachable.size()
        << " celule\n";

    std::map<GhostType, int> perType;
    for (const Ghost& ghost : engine.ghosts()) {
        ++perType[ghost.type];
    }
    out << "std::map<GhostType,int>: ";
    for (const auto& entry : perType) {
        out << entry.first << "=" << entry.second << " ";
    }
    out << "\n";

    // ---------------------------------------------------------------- 3
    title(out, "3. Algoritmi STL");
    std::vector<ScoreEntry> entries{{"Ana", 300}, {"Bob", 900}, {"Cris", 600}, {"Dan", 600}};
    std::sort(entries.begin(), entries.end(), [](const ScoreEntry& x, const ScoreEntry& y) {
        return x.score != y.score ? x.score > y.score : x.name < y.name;
    });
    out << "std::sort (scor descrescator): ";
    std::copy(entries.begin(), entries.end(), std::ostream_iterator<ScoreEntry>(out, " | "));
    out << "\n";

    const auto found = std::find_if(entries.begin(), entries.end(),
                                    [](const ScoreEntry& e) { return e.name == "Cris"; });
    out << "std::find_if: Cris este pe locul " << (found - entries.begin()) + 1 << "\n";

    std::vector<ScoreEntry> highOnes;
    std::copy_if(entries.begin(), entries.end(), std::back_inserter(highOnes),
                 [](const ScoreEntry& e) { return e.score >= 600; });
    out << "std::copy_if (scor >= 600): " << highOnes.size() << " intrari\n";

    out << "std::accumulate (suma): "
        << std::accumulate(entries.begin(), entries.end(), 0,
                           [](int sum, const ScoreEntry& e) { return sum + e.score; })
        << "\n";
    out << "std::equal (copie == original): "
        << std::equal(highOnes.begin(), highOnes.end(), entries.begin(), entries.begin() + 3)
        << "\n";

    // ---------------------------------------------------------------- 4
    title(out, "4. Fluxuri de intrare / iesire");
    std::ostringstream levelDump;
    levelDump << engine;  // GameEngine -> text
    out << "operator<< pentru GameEngine (primele 3 randuri):\n";
    std::istringstream lines(levelDump.str());
    std::string line;
    for (int i = 0; i < 3 && std::getline(lines, line); ++i) {
        out << "  " << line << "\n";
    }

    std::istringstream levelText("; nivel mic\n#####\n#P.o#\n#####\n");
    GameEngine small;
    out << "loadLevel(std::istream&) dintr-un std::istringstream: " << small.loadLevel(levelText)
        << ", " << small.dotsRemaining() << " puncte\n";

    std::istringstream scoreText("Ana 1200\nBob 800\nCris 1500\n");
    HighScores table(5);
    table.load(scoreText);  // std::istream_iterator<ScoreEntry>
    table.print(out);

    // ---------------------------------------------------------------- 5
    title(out, "5. Smart pointers si referinte");
    std::unique_ptr<Renderer> renderer = std::make_unique<ConsoleRenderer>();
    Renderer& ref = *renderer;  // referinta la obiectul detinut
    out << "unique_ptr<Renderer> detine un ConsoleRenderer; Renderer este abstract: "
        << std::is_abstract<Renderer>::value << "\n";
    (void)ref;

    auto config = std::make_shared<const GameConfig>();
    GameEngine first(config);
    GameEngine second(config);
    out << "shared_ptr<const GameConfig>: use_count = " << config.use_count()
        << " (config, first, second)\n";
    out << "std::string in loc de char*: toString(Direction::Left) = \"" << toString(Direction::Left)
        << "\"\n";
    return 0;
}

}  // namespace pacman
