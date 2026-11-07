#include "GameEngine.hpp"

#include <algorithm>
#include <array>
#include <queue>
#include <utility>

namespace pacman {

namespace {

// Ordinea de prioritate la egalitate de distanță (ca în jocul original).
constexpr std::array<Direction, 4> kDirections{Direction::Up, Direction::Left, Direction::Down,
                                               Direction::Right};

}  // namespace

GameEngine::GameEngine(const GameConfig& config) : config_(config), rng_(std::random_device{}()) {
    if (!config_.isValid()) {
        config_ = GameConfig{};
    }
}

bool GameEngine::loadLevel(const std::vector<std::string>& layout) {
    if (layout.empty() || layout.front().empty()) {
        return false;
    }
    const std::size_t width = layout.front().size();
    for (const std::string& row : layout) {
        if (row.size() != width) {
            return false;
        }
    }

    Board board(static_cast<int>(width), static_cast<int>(layout.size()));
    Pacman pacman;
    std::vector<Ghost> ghosts;
    bool pacmanFound = false;
    int dots = 0;

    for (int y = 0; y < board.height; ++y) {
        for (int x = 0; x < board.width; ++x) {
            const Position here{x, y};
            switch (layout[static_cast<std::size_t>(y)][static_cast<std::size_t>(x)]) {
                case '#':
                    board.at(x, y) = CellType::Wall;
                    break;
                case '.':
                    board.at(x, y) = CellType::Dot;
                    ++dots;
                    break;
                case 'o':
                    board.at(x, y) = CellType::PowerPellet;
                    ++dots;
                    break;
                case '-':
                    board.at(x, y) = CellType::GhostHouse;
                    break;
                case 'G': {
                    board.at(x, y) = CellType::GhostHouse;
                    Ghost ghost;
                    ghost.type = static_cast<GhostType>(ghosts.size() % 4);
                    ghost.position = here;
                    ghost.spawn = here;
                    ghosts.push_back(ghost);
                    break;
                }
                case 'P':
                    if (pacmanFound) {
                        return false;  // un singur Pacman
                    }
                    pacmanFound = true;
                    pacman.spawn = here;
                    break;
                case ' ':
                    break;
                default:
                    return false;  // caracter necunoscut
            }
        }
    }
    if (!pacmanFound) {
        return false;
    }

    layout_ = layout;
    board_ = std::move(board);
    pacman_ = pacman;
    pacman_.reset(config_.initialLives);
    ghosts_ = std::move(ghosts);
    dotsRemaining_ = dots;
    tick_ = 0;
    score_.reset();
    state_ = GameState::Menu;
    return true;
}

void GameEngine::start() {
    if (state_ == GameState::Menu && board_.width > 0) {
        state_ = GameState::Running;
    }
}

void GameEngine::reset() {
    if (layout_.empty()) {
        return;
    }
    const std::vector<std::string> layout = layout_;  // copie: loadLevel suprascrie layout_
    loadLevel(layout);
}

void GameEngine::handleCommand(Command command) {
    if (command == Command::None) {
        return;
    }
    if (command == Command::Quit) {
        quit_ = true;
        return;
    }

    const Direction dir = directionFromCommand(command);
    switch (state_) {
        case GameState::Menu:
            if (dir != Direction::None) {
                start();
                pacman_.requestDirection(dir);
            }
            break;
        case GameState::Running:
            if (dir != Direction::None) {
                pacman_.requestDirection(dir);
            } else if (command == Command::Pause) {
                state_ = GameState::Paused;
            }
            break;
        case GameState::Paused:
            if (command == Command::Pause) {
                state_ = GameState::Running;
            }
            break;
        case GameState::Won:
        case GameState::Lost:
            if (dir != Direction::None) {  // reluare joc
                reset();
                start();
            }
            break;
    }
}

void GameEngine::update() {
    if (state_ != GameState::Running) {
        return;
    }
    ++tick_;
    updateGhostModes();

    movePacman();
    checkCollisions();
    if (state_ != GameState::Running) {
        return;
    }

    if (tick_ % config_.ghostSpeedDivider == 0) {
        moveGhosts();
        checkCollisions();
    }
    for (Ghost& ghost : ghosts_) {
        ghost.tick();
    }
}

// ---------- mișcare ----------

Position GameEngine::step(const Position& from, Direction dir) const {
    return board_.wrap(from.moved(dir));
}

bool GameEngine::canMove(const Position& from, Direction dir) const {
    if (dir == Direction::None) {
        return false;
    }
    return board_.isWalkable(step(from, dir), false);
}

bool GameEngine::canGhostMove(const Ghost& ghost, Direction dir) const {
    if (dir == Direction::None) {
        return false;
    }
    // Casa fantomelor poate fi părăsită oricând, dar reintrată doar de fantomele mâncate.
    const bool inHouse = board_.get(ghost.position.x, ghost.position.y) == CellType::GhostHouse;
    const bool allowHouse = inHouse || ghost.mode == GhostMode::Eaten;
    return board_.isWalkable(step(ghost.position, dir), allowHouse);
}

void GameEngine::movePacman() {
    // Virajul cerut se aplică imediat ce se poate.
    if (canMove(pacman_.position, pacman_.nextDirection)) {
        pacman_.direction = pacman_.nextDirection;
    }
    if (canMove(pacman_.position, pacman_.direction)) {
        pacman_.position = step(pacman_.position, pacman_.direction);
        eatAt(pacman_.position);
    }
}

void GameEngine::moveGhosts() {
    for (Ghost& ghost : ghosts_) {
        // Fantomele speriate se mișcă de două ori mai încet.
        if (ghost.mode == GhostMode::Frightened &&
            tick_ % (2 * config_.ghostSpeedDivider) != 0) {
            continue;
        }
        ghost.direction = chooseDirection(ghost);
        if (ghost.direction != Direction::None) {
            ghost.position = step(ghost.position, ghost.direction);
        }
        if (ghost.mode == GhostMode::Eaten && ghost.position == ghost.spawn) {
            ghost.mode = GhostMode::Scatter;  // a revenit în casă
            ghost.direction = Direction::None;
        }
    }
}

void GameEngine::updateGhostModes() {
    const int cycle = config_.scatterTicks + config_.chaseTicks;
    const bool scatter = (tick_ % cycle) < config_.scatterTicks;
    for (Ghost& ghost : ghosts_) {
        if (ghost.mode == GhostMode::Chase || ghost.mode == GhostMode::Scatter) {
            ghost.mode = scatter ? GhostMode::Scatter : GhostMode::Chase;
        }
    }
}

// ---------- inteligența fantomelor ----------

Position GameEngine::scatterCorner(GhostType type) const {
    const int right = board_.width - 1;
    const int bottom = board_.height - 1;
    switch (type) {
        case GhostType::Blinky: return {right, 0};
        case GhostType::Pinky:  return {0, 0};
        case GhostType::Inky:   return {right, bottom};
        case GhostType::Clyde:  return {0, bottom};
    }
    return {0, 0};
}

Position GameEngine::ghostTarget(const Ghost& ghost) const {
    const Position corner = scatterCorner(ghost.type);
    if (ghost.mode == GhostMode::Scatter) {
        return corner;
    }

    const Position pac = pacman_.position;
    const Position off = directionOffset(pacman_.direction);
    switch (ghost.type) {
        case GhostType::Blinky:
            return pac;  // urmărire directă
        case GhostType::Pinky:
            return {pac.x + 4 * off.x, pac.y + 4 * off.y};  // 4 celule în fața lui Pacman
        case GhostType::Inky: {
            const Position ahead{pac.x + 2 * off.x, pac.y + 2 * off.y};
            Position blinky = pac;
            for (const Ghost& other : ghosts_) {
                if (other.type == GhostType::Blinky) {
                    blinky = other.position;
                    break;
                }
            }
            return {2 * ahead.x - blinky.x, 2 * ahead.y - blinky.y};  // simetric față de Blinky
        }
        case GhostType::Clyde:
            return ghost.position.distanceSquared(pac) > 64 ? pac : corner;
    }
    return pac;
}

Direction GameEngine::bfsDirection(const Ghost& ghost, const Position& target) const {
    if (ghost.position == target) {
        return Direction::None;
    }
    const int w = board_.width;
    const auto index = [w](const Position& p) { return static_cast<std::size_t>(p.y * w + p.x); };

    std::vector<bool> visited(static_cast<std::size_t>(w * board_.height), false);
    std::vector<int> firstStep(visited.size(), 0);  // prima direcție spre fiecare celulă
    std::queue<Position> queue;

    visited[index(ghost.position)] = true;
    queue.push(ghost.position);

    while (!queue.empty()) {
        const Position current = queue.front();
        queue.pop();
        for (Direction dir : kDirections) {
            const Position next = step(current, dir);
            if (!board_.isWalkable(next, true) || visited[index(next)]) {
                continue;
            }
            visited[index(next)] = true;
            firstStep[index(next)] =
                (current == ghost.position) ? static_cast<int>(dir) : firstStep[index(current)];
            if (next == target) {
                return static_cast<Direction>(firstStep[index(next)]);
            }
            queue.push(next);
        }
    }
    return Direction::None;
}

Direction GameEngine::chooseDirection(const Ghost& ghost) {
    // O fantomă mâncată se întoarce pe drumul cel mai scurt în casă.
    if (ghost.mode == GhostMode::Eaten) {
        return bfsDirection(ghost, ghost.spawn);
    }

    std::vector<Direction> options;
    for (Direction dir : kDirections) {
        if (dir == opposite(ghost.direction)) {
            continue;  // nu se întoarce pe loc
        }
        if (canGhostMove(ghost, dir)) {
            options.push_back(dir);
        }
    }
    if (options.empty()) {  // fund de sac: singura ieșire e înapoi
        const Direction back = opposite(ghost.direction);
        return canGhostMove(ghost, back) ? back : Direction::None;
    }

    if (ghost.mode == GhostMode::Frightened) {
        std::uniform_int_distribution<std::size_t> pick(0, options.size() - 1);
        return options[pick(rng_)];
    }

    const Position target = ghostTarget(ghost);
    Direction best = options.front();
    int bestDistance = step(ghost.position, best).distanceSquared(target);
    for (Direction dir : options) {
        const int distance = step(ghost.position, dir).distanceSquared(target);
        if (distance < bestDistance) {
            best = dir;
            bestDistance = distance;
        }
    }
    return best;
}

// ---------- reguli ----------

void GameEngine::eatAt(const Position& position) {
    CellType& cell = board_.at(position.x, position.y);
    if (cell == CellType::Dot) {
        score_.add(config_.dotPoints);
        cell = CellType::Empty;
        --dotsRemaining_;
    } else if (cell == CellType::PowerPellet) {
        score_.add(config_.powerPelletPoints);
        cell = CellType::Empty;
        --dotsRemaining_;
        for (Ghost& ghost : ghosts_) {
            ghost.frighten(config_.frightenedDuration);
        }
    }
    if (dotsRemaining_ <= 0 && state_ == GameState::Running) {
        state_ = GameState::Won;
    }
}

void GameEngine::checkCollisions() {
    if (state_ != GameState::Running) {
        return;
    }
    for (Ghost& ghost : ghosts_) {
        if (ghost.position != pacman_.position) {
            continue;
        }
        if (ghost.isVulnerable()) {
            ghost.markEaten();
            score_.add(config_.ghostPoints);
        } else if (ghost.isDangerous()) {
            loseLife();
            return;
        }
    }
}

void GameEngine::loseLife() {
    pacman_.loseLife();
    if (!pacman_.isAlive()) {
        state_ = GameState::Lost;
        return;
    }
    pacman_.respawn();
    for (Ghost& ghost : ghosts_) {
        ghost.reset();
    }
}

}  // namespace pacman
