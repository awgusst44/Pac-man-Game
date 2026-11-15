// Biblioteca pacman_config: clasa GameConfig.
#include "GameConfig.hpp"

#include <istream>
#include <ostream>

#include "StreamParsing.hpp"

namespace pacman {

GameConfig::GameConfig()
    : initialLives(3),
      frightenedDuration(40),
      dotPoints(10),
      powerPelletPoints(50),
      ghostPoints(200),
      scatterTicks(30),
      chaseTicks(80),
      ghostSpeedDivider(2),
      tickMillis(120) {}

GameConfig::GameConfig(int initialLives, int frightenedDuration, int dotPoints,
                       int powerPelletPoints, int ghostPoints)
    : initialLives(initialLives),
      frightenedDuration(frightenedDuration),
      dotPoints(dotPoints),
      powerPelletPoints(powerPelletPoints),
      ghostPoints(ghostPoints),
      scatterTicks(30),
      chaseTicks(80),
      ghostSpeedDivider(2),
      tickMillis(120) {}

GameConfig::GameConfig(const GameConfig& other)
    : initialLives(other.initialLives),
      frightenedDuration(other.frightenedDuration),
      dotPoints(other.dotPoints),
      powerPelletPoints(other.powerPelletPoints),
      ghostPoints(other.ghostPoints),
      scatterTicks(other.scatterTicks),
      chaseTicks(other.chaseTicks),
      ghostSpeedDivider(other.ghostSpeedDivider),
      tickMillis(other.tickMillis) {}

GameConfig& GameConfig::operator=(const GameConfig& other) {
    if (this != &other) {
        initialLives = other.initialLives;
        frightenedDuration = other.frightenedDuration;
        dotPoints = other.dotPoints;
        powerPelletPoints = other.powerPelletPoints;
        ghostPoints = other.ghostPoints;
        scatterTicks = other.scatterTicks;
        chaseTicks = other.chaseTicks;
        ghostSpeedDivider = other.ghostSpeedDivider;
        tickMillis = other.tickMillis;
    }
    return *this;
}

bool GameConfig::operator==(const GameConfig& other) const {
    return initialLives == other.initialLives && frightenedDuration == other.frightenedDuration &&
           dotPoints == other.dotPoints && powerPelletPoints == other.powerPelletPoints &&
           ghostPoints == other.ghostPoints && scatterTicks == other.scatterTicks &&
           chaseTicks == other.chaseTicks && ghostSpeedDivider == other.ghostSpeedDivider &&
           tickMillis == other.tickMillis;
}
bool GameConfig::operator!=(const GameConfig& other) const { return !(*this == other); }

bool GameConfig::isValid() const {
    return initialLives > 0 && frightenedDuration > 0 && dotPoints >= 0 && powerPelletPoints >= 0 &&
           ghostPoints >= 0 && scatterTicks > 0 && chaseTicks > 0 && ghostSpeedDivider > 0 &&
           tickMillis > 0;
}

std::ostream& operator<<(std::ostream& out, const GameConfig& config) {
    return out << "GameConfig lives=" << config.initialLives
               << " frightened=" << config.frightenedDuration << " dot=" << config.dotPoints
               << " pellet=" << config.powerPelletPoints << " ghost=" << config.ghostPoints
               << " scatter=" << config.scatterTicks << " chase=" << config.chaseTicks
               << " divider=" << config.ghostSpeedDivider << " tick=" << config.tickMillis;
}

std::istream& operator>>(std::istream& in, GameConfig& config) {
    using namespace detail;
    GameConfig tmp;
    if (expectKeyword(in, "GameConfig") && intField(in, "lives", tmp.initialLives) &&
        intField(in, "frightened", tmp.frightenedDuration) && intField(in, "dot", tmp.dotPoints) &&
        intField(in, "pellet", tmp.powerPelletPoints) && intField(in, "ghost", tmp.ghostPoints) &&
        intField(in, "scatter", tmp.scatterTicks) && intField(in, "chase", tmp.chaseTicks) &&
        intField(in, "divider", tmp.ghostSpeedDivider) && intField(in, "tick", tmp.tickMillis)) {
        config = tmp;
    }
    return in;
}

}  // namespace pacman
