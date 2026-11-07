#include "HighScores.hpp"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <istream>
#include <iterator>
#include <numeric>
#include <ostream>

namespace pacman {

// ---------- ScoreEntry ----------

ScoreEntry::ScoreEntry() : name(), score(0) {}

ScoreEntry::ScoreEntry(std::string name, int score) : name(std::move(name)), score(score) {}

bool ScoreEntry::operator==(const ScoreEntry& other) const {
    return name == other.name && score == other.score;
}
bool ScoreEntry::operator!=(const ScoreEntry& other) const { return !(*this == other); }

std::ostream& operator<<(std::ostream& out, const ScoreEntry& entry) {
    return out << entry.name << ' ' << entry.score;
}

std::istream& operator>>(std::istream& in, ScoreEntry& entry) {
    std::string name;
    int score = 0;
    if (in >> name >> score) {
        entry = ScoreEntry(name, score);  // modificat doar dacă citirea a reușit
    }
    return in;
}

// ---------- HighScores ----------

HighScores::HighScores(std::size_t capacity) : capacity_(std::max<std::size_t>(capacity, 1)), entries_() {}

std::string HighScores::sanitizeName(std::string name) {
    std::replace(name.begin(), name.end(), ' ', '_');
    std::replace(name.begin(), name.end(), '\t', '_');
    if (name.empty()) {
        name = "Player";
    }
    if (name.size() > 12) {
        name.resize(12);
    }
    return name;
}

bool HighScores::qualifies(int score) const {
    return score > 0 && (entries_.size() < capacity_ || score > entries_.back().score);
}

void HighScores::normalize() {
    // Scor descrescător; la scoruri egale, ordine alfabetică.
    std::sort(entries_.begin(), entries_.end(), [](const ScoreEntry& a, const ScoreEntry& b) {
        return a.score != b.score ? a.score > b.score : a.name < b.name;
    });
    if (entries_.size() > capacity_) {
        entries_.erase(entries_.begin() + static_cast<std::ptrdiff_t>(capacity_), entries_.end());
    }
}

bool HighScores::add(const std::string& name, int score) {
    if (!qualifies(score)) {
        return false;
    }
    const ScoreEntry entry(sanitizeName(name), score);
    entries_.push_back(entry);
    normalize();
    return std::find(entries_.begin(), entries_.end(), entry) != entries_.end();
}

std::optional<ScoreEntry> HighScores::best() const {
    if (entries_.empty()) {
        return std::nullopt;
    }
    return entries_.front();
}

std::size_t HighScores::rankOf(const std::string& name) const {
    const std::string clean = sanitizeName(name);
    const auto found = std::find_if(entries_.begin(), entries_.end(),
                                    [&clean](const ScoreEntry& e) { return e.name == clean; });
    return found == entries_.end() ? 0 : static_cast<std::size_t>(found - entries_.begin()) + 1;
}

std::vector<ScoreEntry> HighScores::forPlayer(const std::string& name) const {
    const std::string clean = sanitizeName(name);
    std::vector<ScoreEntry> result;
    std::copy_if(entries_.begin(), entries_.end(), std::back_inserter(result),
                 [&clean](const ScoreEntry& e) { return e.name == clean; });
    return result;
}

int HighScores::total() const {
    return std::accumulate(entries_.begin(), entries_.end(), 0,
                           [](int sum, const ScoreEntry& e) { return sum + e.score; });
}

void HighScores::save(std::ostream& out) const {
    std::copy(entries_.begin(), entries_.end(), std::ostream_iterator<ScoreEntry>(out, "\n"));
}

void HighScores::load(std::istream& in) {
    std::vector<ScoreEntry> loaded;
    std::copy(std::istream_iterator<ScoreEntry>(in), std::istream_iterator<ScoreEntry>(),
              std::back_inserter(loaded));
    // Intrările negative sau fără nume valid sunt ignorate.
    loaded.erase(std::remove_if(loaded.begin(), loaded.end(),
                                [](const ScoreEntry& e) { return e.score < 0 || e.name.empty(); }),
                 loaded.end());
    entries_ = std::move(loaded);
    normalize();
}

bool HighScores::saveToFile(const std::string& path) const {
    std::ofstream file(path);
    if (!file) {
        return false;
    }
    save(file);
    return static_cast<bool>(file);
}

bool HighScores::loadFromFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        return false;
    }
    load(file);
    return true;
}

void HighScores::print(std::ostream& out) const {
    out << "=== TOP " << capacity_ << " SCORURI ===\n";
    if (entries_.empty()) {
        out << "(niciun scor inca)\n";
        return;
    }
    int place = 1;
    for (const ScoreEntry& entry : entries_) {
        out << std::setw(2) << place++ << ". " << std::left << std::setw(13) << entry.name
            << std::right << std::setw(7) << entry.score << '\n';
    }
}

bool HighScores::operator==(const HighScores& other) const {
    return capacity_ == other.capacity_ &&
           std::equal(entries_.begin(), entries_.end(), other.entries_.begin(), other.entries_.end());
}
bool HighScores::operator!=(const HighScores& other) const { return !(*this == other); }

}  // namespace pacman
