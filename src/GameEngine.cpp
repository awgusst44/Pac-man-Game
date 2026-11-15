#include "GameEngine.hpp"

#include <algorithm>
#include <array>
#include <istream>
#include <iterator>
#include <map>
#include <ostream>
#include <queue>
#include <set>
#include <utility>

namespace pacman {

namespace {

// Ordinea de prioritate la egalitate de distanță (ca în jocul original).
const std::array<Direction, 4> kDirections{
    {Direction::Up, Direction::Left, Direction::Down, Direction::Right}};

}  // namespace

GameEngine::GameEngine(const GameConfig& config)
    : GameEngine(std::make_shared<const GameConfig>(config)) {}

GameEngine::GameEngine(std::shared_ptr<const GameConfig> config)
    : config_(std::move(config)), rng_(std::random_device{}()) {
    if (!config_ || !config_->isValid()) {
        config_ = std::make_shared<const GameConfig>();  // configurație implicită
    }
}

// ---------- încărcarea nivelului ----------

bool GameEngine::loadLevel(const std::vector<std::string>& layout) {
    if (layout.empty() || layout.front().empty()) {
        return false;
    }
    const std::size_t width = layout.front().size();
    const bool sameWidth = std::all_of(layout.begin(), layout.end(), [width](const std::string& row) {
        return row.size() == width;
    });
    if (!sameWidth) {
        return false;
    }

    Board board(static_cast<int>(width), static_cast<int>(layout.size()));
    std::vector<Ghost> ghosts;
    Position pacmanSpawn;
    bool pacmanFound = false;

    for (int y = 0; y < board.height; ++y) {
        const std::string& row = layout[static_cast<std::size_t>(y)];
        for (int x = 0; x < board.width; ++x) {
            const Position here(x, y);
            switch (row[static_cast<std::size_t>(x)]) {
                case '#':
                    board.at(x, y) = CellType::Wall;
                    break;
                case '.':
                    board.at(x, y) = CellType::Dot;
                    break;
                case 'o':
                    board.at(x, y) = CellType::PowerPellet;
                    break;
                case '-':
                    board.at(x, y) = CellType::GhostHouse;
                    break;
                case 'G':
                    board.at(x, y) = CellType::GhostHouse;
                    ghosts.emplace_back(static_cast<GhostType>(ghosts.size() % 4), here);
                    break;
                case 'P':
                    if (pacmanFound) {
                        return false;  // un singur Pacman
                    }
                    pacmanFound = true;
                    pacmanSpawn = here;
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

    // Toate punctele și bilele trebuie să poată fi mâncate: food ⊆ reachable.
    const std::set<Position> dots = board.positionsOf(CellType::Dot);
    const std::set<Position> pellets = board.positionsOf(CellType::PowerPellet);
    std::set<Position> food;
    std::set_union(dots.begin(), dots.end(), pellets.begin(), pellets.end(),
                   std::inserter(food, food.begin()));
    const std::set<Position> reachable = board.reachableFrom(pacmanSpawn);
    if (!std::includes(reachable.begin(), reachable.end(), food.begin(), food.end())) {
        return false;
    }

    layout_ = layout;
    board_ = std::move(board);
    pacman_ = Pacman(pacmanSpawn, config_->initialLives);
    ghosts_ = std::move(ghosts);
    dotsRemaining_ = static_cast<int>(food.size());
    tick_ = 0;
    score_.reset();
    state_ = GameState::Menu;
    return true;
}

bool GameEngine::loadLevel(std::istream& in) {
    std::vector<std::string> rows;
    std::string line;
    while (std::getline(in, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();  // fișiere cu sfârșit de linie Windows
        }
        if (!line.empty() && line.front() == ';') {
            continue;  // comentariu
        }
        rows.push_back(line);
    }

    // Elimină rândurile goale de la început și de la sfârșit.
    const auto isBlank = [](const std::string& row) { return row.empty(); };
    rows.erase(rows.begin(), std::find_if_not(rows.begin(), rows.end(), isBlank));
    rows.erase(std::find_if_not(rows.rbegin(), rows.rend(), isBlank).base(), rows.end());
    if (rows.empty()) {
        return false;
    }

    // Rândurile mai scurte (spațiile de la sfârșit pot dispărea la editare) se completează.
    const std::size_t width = std::max_element(rows.begin(), rows.end(),
                                               [](const std::string& a, const std::string& b) {
                                                   return a.size() < b.size();
                                               })->size();
    std::for_each(rows.begin(), rows.end(), [width](std::string& row) { row.resize(width, ' '); });

    return loadLevel(rows);
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

    if (tick_ % config_->ghostSpeedDivider == 0) {
        moveGhosts();
        checkCollisions();
    }
    std::for_each(ghosts_.begin(), ghosts_.end(), [](Ghost& ghost) { ghost.tick(); });
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
            tick_ % (2 * config_->ghostSpeedDivider) != 0) {
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
    const int cycle = config_->scatterTicks + config_->chaseTicks;
    const bool scatter = (tick_ % cycle) < config_->scatterTicks;
    std::for_each(ghosts_.begin(), ghosts_.end(), [scatter](Ghost& ghost) {
        if (ghost.mode == GhostMode::Chase || ghost.mode == GhostMode::Scatter) {
            ghost.mode = scatter ? GhostMode::Scatter : GhostMode::Chase;
        }
    });
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
            const auto blinky = std::find_if(ghosts_.begin(), ghosts_.end(), [](const Ghost& g) {
                return g.type == GhostType::Blinky;
            });
            const Position b = (blinky != ghosts_.end()) ? blinky->position : pac;
            return {2 * ahead.x - b.x, 2 * ahead.y - b.y};  // simetric față de Blinky
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
    // Pentru fiecare celulă vizitată: prima direcție de urmat din poziția fantomei spre ea.
    std::map<Position, Direction> firstStep;
    std::queue<Position> pending;
    firstStep.emplace(ghost.position, Direction::None);
    pending.push(ghost.position);

    while (!pending.empty()) {
        const Position current = pending.front();
        pending.pop();
        for (Direction dir : kDirections) {
            const Position next = step(current, dir);
            if (!board_.isWalkable(next, true) || firstStep.count(next) > 0) {
                continue;
            }
            const Direction first = (current == ghost.position) ? dir : firstStep.at(current);
            firstStep.emplace(next, first);
            if (next == target) {
                return first;
            }
            pending.push(next);
        }
    }
    return Direction::None;
}

Direction GameEngine::chooseDirection(const Ghost& ghost) {
    // O fantomă mâncată se întoarce pe drumul cel mai scurt în casă.
    if (ghost.mode == GhostMode::Eaten) {
        return bfsDirection(ghost, ghost.spawn);
    }

    // Direcțiile posibile: nu se întoarce pe loc și nu intră în perete.
    std::vector<Direction> options;
    std::copy_if(kDirections.begin(), kDirections.end(), std::back_inserter(options),
                 [&](Direction dir) {
                     return dir != opposite(ghost.direction) && canGhostMove(ghost, dir);
                 });
    if (options.empty()) {  // fund de sac: singura ieșire e înapoi
        const Direction back = opposite(ghost.direction);
        return canGhostMove(ghost, back) ? back : Direction::None;
    }

    if (ghost.mode == GhostMode::Frightened) {
        std::uniform_int_distribution<std::size_t> pick(0, options.size() - 1);
        return options[pick(rng_)];
    }

    // Alege direcția care apropie cel mai mult fantoma de țintă.
    const Position target = ghostTarget(ghost);
    const auto distance = [&](Direction dir) {
        return step(ghost.position, dir).distanceSquared(target);
    };
    return *std::min_element(options.begin(), options.end(),
                             [&](Direction a, Direction b) { return distance(a) < distance(b); });
}

// ---------- reguli ----------

void GameEngine::eatAt(const Position& position) {
    CellType& cell = board_.at(position.x, position.y);
    if (cell == CellType::Dot) {
        score_.add(config_->dotPoints);
        cell = CellType::Empty;
        --dotsRemaining_;
    } else if (cell == CellType::PowerPellet) {
        score_.add(config_->powerPelletPoints);
        cell = CellType::Empty;
        --dotsRemaining_;
        const int duration = config_->frightenedDuration;
        std::for_each(ghosts_.begin(), ghosts_.end(),
                      [duration](Ghost& ghost) { ghost.frighten(duration); });
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
            score_.add(config_->ghostPoints);
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
    std::for_each(ghosts_.begin(), ghosts_.end(), [](Ghost& ghost) { ghost.reset(); });
}

// ---------- afișare ----------

std::ostream& operator<<(std::ostream& out, const GameEngine& engine) {
    const Board& board = engine.board();
    const std::vector<Ghost>& ghosts = engine.ghosts();
    for (int y = 0; y < board.height; ++y) {
        for (int x = 0; x < board.width; ++x) {
            const Position here(x, y);
            const bool ghostHere = std::any_of(ghosts.begin(), ghosts.end(), [&here](const Ghost& g) {
                return g.position == here;
            });
            if (engine.pacman().position == here) {
                out << 'P';
            } else if (ghostHere) {
                out << 'G';
            } else {
                out << toChar(board.at(x, y));
            }
        }
        out << '\n';
    }
    return out;
}

}  // namespace pacman
