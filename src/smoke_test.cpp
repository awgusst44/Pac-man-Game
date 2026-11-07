// Teste simple (fără biblioteci externe) pentru structurile și motorul Pacman.
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <type_traits>
#include <vector>

#include "GameEngine.hpp"
#include "Renderer.hpp"
#include "Types.hpp"

namespace {

int g_failures = 0;

#define CHECK(cond)                                                                \
    do {                                                                           \
        if (!(cond)) {                                                             \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #cond "\n"; \
            ++g_failures;                                                          \
        }                                                                          \
    } while (0)

using namespace pacman;

void testTypes() {
    CHECK((Position{1, 1}.moved(Direction::Up) == Position{1, 0}));
    CHECK((Position{0, 0}.distanceSquared(Position{3, 4}) == 25));
    CHECK(opposite(Direction::Left) == Direction::Right);
    CHECK(opposite(Direction::None) == Direction::None);
    CHECK(directionFromCommand(Command::MoveDown) == Direction::Down);
    CHECK(directionFromCommand(Command::Pause) == Direction::None);

    Board board(3, 2, CellType::Dot);
    CHECK(board.cells.size() == 6);
    CHECK((board.wrap(Position{-1, 0}) == Position{2, 0}));
    CHECK((board.wrap(Position{3, 2}) == Position{0, 0}));
    CHECK(board.count(CellType::Dot) == 6);
    board.at(1, 1) = CellType::Wall;
    CHECK(!board.isWalkable(Position{1, 1}, false));
    CHECK(board.get(10, 10) == CellType::Wall);
    board.at(0, 0) = CellType::GhostHouse;
    CHECK(!board.isWalkable(Position{0, 0}, false));
    CHECK(board.isWalkable(Position{0, 0}, true));

    Score score;
    score.add(100);
    score.reset();
    CHECK(score.current == 0 && score.best == 100);

    Ghost ghost;
    ghost.direction = Direction::Left;
    ghost.frighten(2);
    CHECK(ghost.isVulnerable() && ghost.direction == Direction::Right);
    ghost.tick();
    ghost.tick();
    CHECK(ghost.isDangerous());
    ghost.markEaten();
    ghost.frighten(5);
    CHECK(ghost.mode == GhostMode::Eaten);

    Pacman pacman;
    pacman.spawn = {2, 3};
    pacman.reset(2);
    pacman.loseLife();
    CHECK(pacman.isAlive());
    pacman.loseLife();
    CHECK(!pacman.isAlive());
    CHECK((pacman.position == Position{2, 3}));

    GameConfig config;
    CHECK(config.isValid());
    config.initialLives = 0;
    CHECK(!config.isValid());
}

void testLoadLevel() {
    GameEngine engine;
    CHECK(!engine.loadLevel({}));
    CHECK(!engine.loadLevel({"#####", "#P.#"}));     // rânduri de lungimi diferite
    CHECK(!engine.loadLevel({"#####", "#...#"}));    // fără Pacman
    CHECK(!engine.loadLevel({"#####", "#PP.#"}));    // doi Pacman
    CHECK(!engine.loadLevel({"#####", "#P?.#"}));    // caracter necunoscut
    CHECK(engine.loadLevel({"#####", "#Po.#", "#####"}));
    CHECK(engine.dotsRemaining() == 2);
    CHECK(engine.state() == GameState::Menu);
}


void testConstructors() {
    // implicit
    Position p;
    CHECK(p.x == 0 && p.y == 0);
    Pacman pac;
    CHECK(pac.lives == 3 && pac.position == Position() && pac.direction == Direction::None);
    Ghost ghost;
    CHECK(ghost.type == GhostType::Blinky && ghost.mode == GhostMode::Scatter);
    Board emptyBoard;
    CHECK(emptyBoard.width == 0 && emptyBoard.height == 0 && emptyBoard.cells.empty());
    Score score;
    CHECK(score.current == 0 && score.best == 0);
    GameConfig config;
    CHECK(config.isValid() && config.initialLives == 3 && config.tickMillis == 120);

    // cu parametri
    Position q(3, 4);
    CHECK(q.x == 3 && q.y == 4);
    Pacman pac2(Position(5, 6), 2);
    CHECK(pac2.position == Position(5, 6) && pac2.spawn == Position(5, 6) && pac2.lives == 2);
    CHECK(Pacman(Position(1, 1), -4).lives == 0);  // valoare invalidă corectată
    Ghost ghost2(GhostType::Clyde, Position(7, 8));
    CHECK(ghost2.type == GhostType::Clyde && ghost2.position == Position(7, 8) &&
          ghost2.spawn == Position(7, 8));
    Board board(4, 3, CellType::Dot);
    CHECK(board.width == 4 && board.height == 3 && board.count(CellType::Dot) == 12);
    CHECK(Board(-2, 5).cells.empty());
    Score score2(10, 5);
    CHECK(score2.current == 10 && score2.best == 10);  // recordul nu poate fi sub scor
    GameConfig config2(5, 60, 1, 2, 3);
    CHECK(config2.initialLives == 5 && config2.frightenedDuration == 60 && config2.dotPoints == 1 &&
          config2.powerPelletPoints == 2 && config2.ghostPoints == 3 && config2.scatterTicks == 30);

    // de copiere
    Position r(q);
    CHECK(r == q);
    Pacman pac3(pac2);
    pac3.loseLife();
    CHECK(pac3.lives == 1 && pac2.lives == 2);  // copiile sunt independente
    Ghost ghost3(ghost2);
    CHECK(ghost3 == ghost2);
    Board boardCopy(board);
    boardCopy.at(0, 0) = CellType::Wall;
    CHECK(board.at(0, 0) == CellType::Dot);  // copie profundă
    CHECK(boardCopy != board);
    Score score3(score2);
    CHECK(score3 == score2);
    GameConfig config3(config2);
    CHECK(config3 == config2);
}

void testAssignment() {
    Position a(1, 2);
    Position b;
    b = a;
    CHECK(b == a);
    Position& self = a;
    a = self;  // auto-atribuire
    CHECK((a == Position(1, 2)));

    Pacman p1(Position(2, 2), 1);
    Pacman p2;
    p2 = p1;
    CHECK(p2 == p1);

    Ghost g1(GhostType::Inky, Position(3, 3));
    g1.frighten(9);
    Ghost g2;
    g2 = g1;
    CHECK(g2 == g1 && g2.isVulnerable());

    Board b1(3, 3, CellType::Dot);
    Board b2;
    b2 = b1;
    b2.at(1, 1) = CellType::Wall;
    CHECK(b1.at(1, 1) == CellType::Dot);  // fără memorie partajată
    b1 = b1;                              // auto-atribuire
    CHECK(b1.cells.size() == 9);

    Score s1(40, 90);
    Score s2;
    s2 = s1;
    CHECK(s2 == s1);

    GameConfig c1(2, 10, 1, 1, 1);
    GameConfig c2;
    c2 = c1;
    CHECK(c2 == c1);

    // atribuire înlănțuită
    Position x, y, z(8, 9);
    x = y = z;
    CHECK(x == z && y == z);
}

void testComparison() {
    CHECK(Position(1, 2) == Position(1, 2));
    CHECK(Position(1, 2) != Position(2, 1));
    CHECK(Position(1, 9) < Position(2, 0));
    CHECK(Position(1, 2) < Position(1, 3));
    CHECK(Position(2, 0) > Position(1, 9));
    CHECK(Position(1, 2) <= Position(1, 2));
    CHECK(Position(1, 2) >= Position(1, 2));
    CHECK(!(Position(1, 2) < Position(1, 2)));

    CHECK(Score(100, 500) > Score(50, 500));
    CHECK(Score(50, 500) < Score(50, 600));
    CHECK(Score(50, 500) <= Score(50, 500));
    CHECK(Score(10, 20) != Score(10, 30));

    Pacman a(Position(1, 1), 3);
    Pacman b(Position(1, 1), 3);
    CHECK(a == b);
    b.requestDirection(Direction::Up);
    CHECK(a != b);

    Ghost g1(GhostType::Pinky, Position(2, 2));
    Ghost g2(GhostType::Pinky, Position(2, 2));
    CHECK(g1 == g2);
    g2.markEaten();
    CHECK(g1 != g2);

    Board b1(2, 2);
    Board b2(2, 2);
    CHECK(b1 == b2);
    b2.at(1, 1) = CellType::Dot;
    CHECK(b1 != b2);
    CHECK(Board(2, 3) != Board(3, 2));

    GameConfig c1;
    GameConfig c2;
    CHECK(c1 == c2);
    c2.tickMillis = 50;
    CHECK(c1 != c2);
}

// Scrie obiectul, îl citește înapoi și verifică egalitatea.
template <typename T>
void roundTrip(const T& original, const char* name) {
    std::stringstream stream;
    stream << original;
    T restored;
    stream >> restored;
    if (!stream || !(restored == original)) {
        std::cerr << "FAIL roundTrip " << name << "\n";
        ++g_failures;
    }
}

void testStreams() {
    // enum-uri
    std::ostringstream out;
    out << Direction::Left << ' ' << GhostType::Inky << ' ' << GhostMode::Frightened << ' '
        << GameState::Won << ' ' << CellType::PowerPellet;
    CHECK(out.str() == "Left Inky Frightened Won o");

    std::istringstream enumIn("Down Clyde Eaten");
    Direction d = Direction::None;
    GhostType t = GhostType::Blinky;
    GhostMode m = GhostMode::Chase;
    enumIn >> d >> t >> m;
    CHECK(d == Direction::Down && t == GhostType::Clyde && m == GhostMode::Eaten);

    std::istringstream badEnum("Sideways");
    Direction keep = Direction::Up;
    badEnum >> keep;
    CHECK(!badEnum && keep == Direction::Up);

    // Position
    std::ostringstream posOut;
    posOut << Position(-3, 12);
    CHECK(posOut.str() == "(-3,12)");
    roundTrip(Position(7, 8), "Position");
    std::istringstream spaced("  ( 5 , 6 )");
    Position sp;
    spaced >> sp;
    CHECK(spaced && sp == Position(5, 6));

    // Pacman, Ghost
    Pacman pac(Position(4, 5), 2);
    pac.direction = Direction::Left;
    pac.nextDirection = Direction::Up;
    roundTrip(pac, "Pacman");
    std::ostringstream pacOut;
    pacOut << pac;
    CHECK(pacOut.str() == "Pacman pos=(4,5) spawn=(4,5) dir=Left next=Up lives=2");

    Ghost ghost(GhostType::Inky, Position(1, 2));
    ghost.frighten(12);
    roundTrip(ghost, "Ghost");

    // Score, GameConfig
    roundTrip(Score(120, 450), "Score");
    GameConfig config(4, 55, 11, 51, 201);
    config.tickMillis = 99;
    roundTrip(config, "GameConfig");

    // Board
    Board board(5, 3, CellType::Dot);
    board.at(0, 0) = CellType::Wall;
    board.at(2, 1) = CellType::PowerPellet;
    board.at(3, 1) = CellType::GhostHouse;
    board.at(4, 2) = CellType::Empty;
    roundTrip(board, "Board");
    std::ostringstream boardOut;
    boardOut << board;
    CHECK(boardOut.str() == "Board 5x3\n#....\n..o-.\n.... \n");

    // citire cu sfârșit de linie Windows (\r\n)
    std::istringstream crlf("Board 3x2\r\n#.#\r\n o-\r\n");
    Board winBoard;
    crlf >> winBoard;
    CHECK(crlf && winBoard.at(0, 1) == CellType::Empty && winBoard.at(2, 1) == CellType::GhostHouse);

    // citiri greșite: setează failbit și NU modifică obiectul
    Position posKeep(9, 9);
    std::istringstream bad1("(1;2)");
    bad1 >> posKeep;
    CHECK(!bad1 && posKeep == Position(9, 9));

    Pacman pacKeep(Position(2, 2), 3);
    std::istringstream bad2("Pacman pos=(1,1) spawn=(1,1) dir=Nowhere next=Up lives=3");
    bad2 >> pacKeep;
    CHECK(!bad2 && pacKeep == Pacman(Position(2, 2), 3));

    Board boardKeep(2, 2, CellType::Dot);
    std::istringstream bad3("Board 3x2\n#.#\n#x#\n");  // 'x' nu e simbol valid
    bad3 >> boardKeep;
    CHECK(!bad3 && boardKeep == Board(2, 2, CellType::Dot));
    std::istringstream bad4("Board 3x2\n#.#\n");       // rânduri lipsă
    bad4 >> boardKeep;
    CHECK(!bad4 && boardKeep == Board(2, 2, CellType::Dot));
    std::istringstream bad5("Board 5000x5000\n");      // dimensiune absurdă
    bad5 >> boardKeep;
    CHECK(!bad5);

    Score scoreKeep(1, 2);
    std::istringstream bad6("Score current=abc best=2");
    bad6 >> scoreKeep;
    CHECK(!bad6 && scoreKeep == Score(1, 2));

    // mai multe obiecte în același flux
    std::stringstream many;
    many << Position(1, 1) << ' ' << Score(2, 3) << ' ' << Position(4, 4);
    Position p1, p2;
    Score s;
    many >> p1 >> s >> p2;
    CHECK(many && p1 == Position(1, 1) && s == Score(2, 3) && p2 == Position(4, 4));
}

// Desenator de test: înregistrează ordinea apelurilor.
class RecordingRenderer : public Renderer {
public:
    std::vector<std::string> calls;
    int ghostsDrawn = 0;

    void init() override { calls.push_back("init"); }
    void shutdown() override { calls.push_back("shutdown"); }

protected:
    void beginFrame() override { calls.push_back("begin"); }
    void drawBoard(const Board&) override { calls.push_back("board"); }
    void drawPacman(const Pacman&) override { calls.push_back("pacman"); }
    void drawGhost(const Ghost&) override {
        ++ghostsDrawn;
        calls.push_back("ghost");
    }
    void drawHud(const Score&, int, int) override { calls.push_back("hud"); }
    void drawStatus(GameState, const Score&) override { calls.push_back("status"); }
    void endFrame() override { calls.push_back("end"); }
};

static_assert(std::is_abstract<Renderer>::value, "Renderer trebuie sa fie clasa abstracta");
static_assert(!std::is_abstract<ConsoleRenderer>::value, "ConsoleRenderer trebuie sa fie concret");
static_assert(!std::is_abstract<RecordingRenderer>::value, "RecordingRenderer trebuie sa fie concret");
static_assert(std::has_virtual_destructor<Renderer>::value, "destructor virtual necesar");
static_assert(!std::is_copy_constructible<Renderer>::value, "Renderer nu se copiaza");

void testAbstractRenderer() {
    GameEngine engine;
    CHECK(engine.loadLevel({"#####", "#PGG#", "#...#", "#####"}));

    RecordingRenderer recorder;
    Renderer& renderer = recorder;  // utilizare polimorfă prin clasa abstractă
    renderer.init();
    renderer.render(engine);
    renderer.shutdown();

    const std::vector<std::string> expected = {"init",   "begin",  "board", "ghost", "ghost",
                                               "pacman", "hud",    "status", "end",  "shutdown"};
    CHECK(recorder.calls == expected);
    CHECK(recorder.ghostsDrawn == 2);

    // prin pointer la bază, cu destructor virtual
    Renderer* heap = new RecordingRenderer();
    heap->render(engine);
    delete heap;
}

void testWin() {
    GameEngine engine;
    CHECK(engine.loadLevel({"#####", "#P..#", "#####"}));
    engine.handleCommand(Command::MoveRight);
    CHECK(engine.isRunning());
    engine.update();
    engine.update();
    CHECK(engine.state() == GameState::Won);
    CHECK(engine.score().current == 20);
    engine.handleCommand(Command::MoveRight);  // joc nou
    CHECK(engine.isRunning() && engine.dotsRemaining() == 2 && engine.score().current == 0);
    CHECK(engine.score().best == 20);
}

void testPauseAndQuit() {
    GameEngine engine;
    CHECK(engine.loadLevel({"#####", "#P..#", "#####"}));
    engine.handleCommand(Command::MoveRight);
    engine.handleCommand(Command::Pause);
    CHECK(engine.state() == GameState::Paused);
    engine.update();
    CHECK(engine.dotsRemaining() == 2);  // în pauză nu se mișcă nimic
    engine.handleCommand(Command::Pause);
    CHECK(engine.isRunning());
    engine.handleCommand(Command::Quit);
    CHECK(engine.quitRequested());
}

void testLose() {
    GameEngine engine;
    CHECK(engine.loadLevel({"########", "#P....G#", "########"}));
    engine.start();  // Pacman stă pe loc, fantoma îl urmărește
    for (int i = 0; i < 5000 && engine.state() == GameState::Running; ++i) {
        engine.update();
    }
    CHECK(engine.state() == GameState::Lost);
    CHECK(engine.pacman().lives == 0);
}

void testPowerPelletAndGhostEaten() {
    GameEngine engine;
    CHECK(engine.loadLevel({"#########", "#Po....G#", "#########"}));
    engine.handleCommand(Command::MoveRight);
    engine.update();  // mănâncă bila de putere
    bool frightened = false;
    for (const Ghost& g : engine.ghosts()) {
        frightened = frightened || g.isVulnerable();
    }
    CHECK(frightened);
    CHECK(engine.score().current == 50);
}

void testRandomPlay() {
    GameEngine engine;
    const std::vector<std::string> level = {
        "#####################", "#o........#........o#", "#.###.###.#.###.###.#",
        "#...................#", "#.###.#.#####.#.###.#", "#.....#...#...#.....#",
        "#####.### # ###.#####", "     .  #GGG#  .     ", "#####.# ##### #.#####",
        "#.....#...#...#.....#", "#.###.###.#.###.###.#", "#o..#.....P.....#..o#",
        "#.###.#.#####.#.###.#", "#...................#", "#####################"};
    CHECK(engine.loadLevel(level));
    CHECK(engine.ghosts().size() == 3);

    std::mt19937 rng(12345);
    const Command commands[] = {Command::MoveUp, Command::MoveDown, Command::MoveLeft,
                                Command::MoveRight, Command::Pause, Command::None, Command::None};
    for (int i = 0; i < 30000; ++i) {
        engine.handleCommand(commands[rng() % 7]);
        engine.update();

        const Board& board = engine.board();
        const Position pac = engine.pacman().position;
        if (!board.isWalkable(pac, false)) {
            std::cerr << "Pacman intr-un perete la pasul " << i << "\n";
            ++g_failures;
            return;
        }
        for (const Ghost& g : engine.ghosts()) {
            if (!board.isWalkable(g.position, true)) {
                std::cerr << "Fantoma intr-un perete la pasul " << i << "\n";
                ++g_failures;
                return;
            }
        }
        const int onBoard = board.count(CellType::Dot) + board.count(CellType::PowerPellet);
        if (onBoard != engine.dotsRemaining()) {
            std::cerr << "dotsRemaining inconsistent la pasul " << i << "\n";
            ++g_failures;
            return;
        }
    }
}

}  // namespace

int main() {
    testTypes();
    testConstructors();
    testAssignment();
    testComparison();
    testStreams();
    testAbstractRenderer();
    testLoadLevel();
    testWin();
    testPauseAndQuit();
    testLose();
    testPowerPelletAndGhostEaten();
    testRandomPlay();

    if (g_failures == 0) {
        std::cout << "Toate testele au trecut.\n";
        return 0;
    }
    std::cerr << g_failures << " verificari esuate.\n";
    return 1;
}
