// Biblioteca pacman_score: clasa Score.
#include "Score.hpp"

#include <algorithm>
#include <istream>
#include <ostream>
#include <tuple>

#include "StreamParsing.hpp"

namespace pacman {

Score::Score() : current(0), best(0) {}

Score::Score(int current, int best) : current(current), best(std::max(best, current)) {}

Score::Score(const Score& other) : current(other.current), best(other.best) {}

Score& Score::operator=(const Score& other) {
    if (this != &other) {
        current = other.current;
        best = other.best;
    }
    return *this;
}

bool Score::operator==(const Score& other) const {
    return current == other.current && best == other.best;
}
bool Score::operator!=(const Score& other) const { return !(*this == other); }
bool Score::operator<(const Score& other) const {
    return std::tie(current, best) < std::tie(other.current, other.best);
}
bool Score::operator>(const Score& other) const { return other < *this; }
bool Score::operator<=(const Score& other) const { return !(other < *this); }
bool Score::operator>=(const Score& other) const { return !(*this < other); }

void Score::add(int points) {
    current += points;
    best = std::max(best, current);
}

void Score::reset() { current = 0; }

std::ostream& operator<<(std::ostream& out, const Score& score) {
    return out << "Score current=" << score.current << " best=" << score.best;
}

std::istream& operator>>(std::istream& in, Score& score) {
    using namespace detail;
    Score tmp;
    if (expectKeyword(in, "Score") && intField(in, "current", tmp.current) &&
        intField(in, "best", tmp.best)) {
        score = tmp;
    }
    return in;
}

}  // namespace pacman
