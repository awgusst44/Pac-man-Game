#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <type_traits>
#include <vector>

#include "GameEngine.hpp"
#include "Listener.hpp"
#include "Renderer.hpp"
#include "Types.hpp"

namespace {

// '#' perete, '.' punct, 'o' bilă de putere, 'P' Pacman, 'G' fantomă, ' ' gol
std::vector<std::string> defaultLevel() {
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

// Demonstrație pentru laborator: constructori, atribuire, comparare, intrare/ieșire.
// Pornire:  pacman --demo
int runDemo() {
    using namespace pacman;
    std::cout << std::boolalpha;

    std::cout << "=== 1. Constructori si atribuire ===\n";
    Position a;                 // constructor implicit
    Position b(3, 4);           // constructor cu parametri
    Position c(b);              // constructor de copiere
    std::cout << "a = " << a << "   b = " << b << "   c = " << c << "\n";
    a = b;                      // operator de atribuire
    std::cout << "dupa a = b:  a = " << a << "\n";

    Pacman pacman(Position(10, 11), 2);
    Pacman pacmanCopy(pacman);
    pacmanCopy.loseLife();
    std::cout << pacman << "\n" << pacmanCopy << "   (copia e independenta)\n";

    Ghost ghost(GhostType::Pinky, Position(5, 6));
    Ghost ghostCopy;
    ghostCopy = ghost;
    std::cout << ghost << "\n" << ghostCopy << "\n";

    Board board(4, 2, CellType::Dot);
    Board boardCopy(board);     // copie profunda
    boardCopy.at(0, 0) = CellType::Wall;
    std::cout << "board:\n" << board << "boardCopy (modificat):\n" << boardCopy;

    Score s1(100, 500);
    Score s2;
    s2 = s1;
    GameConfig config(5, 60, 10, 50, 200);
    std::cout << s1 << "\n" << s2 << "\n" << config << "\n\n";

    std::cout << "=== 2. Operatori de comparare ===\n";
    std::cout << "b == c            -> " << (b == c) << "\n";
    std::cout << "b != Position(0,0) -> " << (b != Position(0, 0)) << "\n";
    std::cout << "Position(1,9) < Position(2,0) -> " << (Position(1, 9) < Position(2, 0)) << "\n";
    std::cout << "pacman == pacmanCopy -> " << (pacman == pacmanCopy) << "\n";
    std::cout << "ghost == ghostCopy   -> " << (ghost == ghostCopy) << "\n";
    std::cout << "board == boardCopy   -> " << (board == boardCopy) << "\n";
    std::cout << "Score(100,500) > Score(50,500) -> " << (Score(100, 500) > Score(50, 500)) << "\n\n";

    std::cout << "=== 3. Operatori de intrare / iesire ===\n";
    std::stringstream stream;
    stream << pacman;           // ieșire în stream
    Pacman restored;
    stream >> restored;         // intrare din stream
    std::cout << "scris si citit inapoi: " << restored << "\n";
    std::cout << "identic cu originalul: " << (restored == pacman) << "\n";

    std::istringstream ghostText(
        "Ghost type=Inky mode=Frightened pos=(1,2) spawn=(3,4) dir=Left frightened=12");
    Ghost parsed;
    ghostText >> parsed;
    std::cout << "citit din text:        " << parsed << "\n";

    std::istringstream boardText("Board 5x3\n#####\n#o.-#\n#####\n");
    Board parsedBoard;
    boardText >> parsedBoard;
    std::cout << "grila citita:\n" << parsedBoard;

    std::istringstream bad("(7;8)");
    Position unchanged(1, 1);
    bad >> unchanged;
    std::cout << "format gresit '(7;8)': citire reusita = " << static_cast<bool>(bad)
              << ", obiectul ramane " << unchanged << "\n\n";

    std::cout << "=== 4. Clasa abstracta Renderer ===\n";
    std::cout << "Renderer este abstract:      " << std::is_abstract<Renderer>::value << "\n";
    std::cout << "ConsoleRenderer e abstract:  " << std::is_abstract<ConsoleRenderer>::value << "\n";
    std::cout << "(Renderer r; nu compileaza; se poate folosi doar prin Renderer& / Renderer*)\n";
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc > 1 && std::strcmp(argv[1], "--demo") == 0) {
        return runDemo();
    }

    using namespace pacman;

    const GameConfig config;
    GameEngine engine(config);
    if (!engine.loadLevel(defaultLevel())) {
        std::cerr << "Nivelul nu a putut fi incarcat.\n";
        return 1;
    }

    ConsoleRenderer consoleRenderer;
    Renderer& renderer = consoleRenderer;  // codul de joc depinde doar de clasa abstractă
    ConsoleListener listener;

    renderer.init();
    while (!engine.quitRequested()) {
        engine.handleCommand(listener.poll());
        engine.update();
        renderer.render(engine);
        std::this_thread::sleep_for(std::chrono::milliseconds(config.tickMillis));
    }
    renderer.shutdown();
    return 0;
}
