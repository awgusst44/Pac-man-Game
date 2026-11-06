#include <chrono>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "GameEngine.hpp"
#include "Listener.hpp"
#include "Renderer.hpp"

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

}  // namespace

int main() {
    using namespace pacman;

    const GameConfig config;
    GameEngine engine(config);
    if (!engine.loadLevel(defaultLevel())) {
        std::cerr << "Nivelul nu a putut fi incarcat.\n";
        return 1;
    }

    ConsoleRenderer renderer;
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
