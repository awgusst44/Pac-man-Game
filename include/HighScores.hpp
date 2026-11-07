#pragma once

#include <cstddef>
#include <iosfwd>
#include <optional>
#include <string>
#include <vector>

namespace pacman {

// O intrare din tabela de scoruri: numele jucătorului (un singur cuvânt) și scorul.
struct ScoreEntry {
    std::string name;
    int score{0};

    ScoreEntry();
    ScoreEntry(std::string name, int score);

    bool operator==(const ScoreEntry& other) const;
    bool operator!=(const ScoreEntry& other) const;
};

// Format în fișier / flux:  "Ana 1200"
std::ostream& operator<<(std::ostream& out, const ScoreEntry& entry);
std::istream& operator>>(std::istream& in, ScoreEntry& entry);

// Tabela celor mai bune scoruri, ordonată descrescător.
class HighScores {
public:
    explicit HighScores(std::size_t capacity = 5);

    // Spații -> '_', nume gol -> "Player", maximum 12 caractere.
    static std::string sanitizeName(std::string name);

    // Un scor intră în tabelă dacă este pozitiv și bate ultimul loc (sau tabela nu e plină).
    bool qualifies(int score) const;
    // Adaugă scorul, sortează și păstrează doar primele `capacity` intrări.
    // Returnează true dacă intrarea a rămas în tabelă.
    bool add(const std::string& name, int score);

    const std::vector<ScoreEntry>& entries() const { return entries_; }
    std::size_t capacity() const { return capacity_; }
    bool empty() const { return entries_.empty(); }

    std::optional<ScoreEntry> best() const;                              // primul loc, dacă există
    std::size_t rankOf(const std::string& name) const;                   // locul (de la 1) sau 0
    std::vector<ScoreEntry> forPlayer(const std::string& name) const;    // toate scorurile unui jucător
    int total() const;                                                   // suma scorurilor

    // Citire / scriere prin fluxuri (un rând pe intrare).
    void save(std::ostream& out) const;
    void load(std::istream& in);
    bool saveToFile(const std::string& path) const;
    bool loadFromFile(const std::string& path);

    // Afișare ca tabel citibil.
    void print(std::ostream& out) const;

    bool operator==(const HighScores& other) const;
    bool operator!=(const HighScores& other) const;

private:
    void normalize();  // sortează descrescător și taie la capacitate

    std::size_t capacity_;
    std::vector<ScoreEntry> entries_;
};

}  // namespace pacman
