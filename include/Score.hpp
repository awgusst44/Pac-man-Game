#pragma once

#include <iosfwd>

namespace pacman {

struct Score {
    int current{0};
    int best{0};

    Score();                                   // implicit: 0 / 0
    Score(int current, int best);              // cu parametri
    Score(const Score& other);                 // de copiere
    Score& operator=(const Score& other);      // de atribuire

    // Comparare lexicografică: întâi scorul curent, apoi recordul.
    bool operator==(const Score& other) const;
    bool operator!=(const Score& other) const;
    bool operator<(const Score& other) const;
    bool operator>(const Score& other) const;
    bool operator<=(const Score& other) const;
    bool operator>=(const Score& other) const;

    void add(int points);  // adaugă puncte și actualizează recordul
    void reset();          // scor curent 0, recordul se păstrează
};

// Format:  Score current=10 best=20
std::ostream& operator<<(std::ostream& out, const Score& score);
std::istream& operator>>(std::istream& in, Score& score);

}  // namespace pacman
