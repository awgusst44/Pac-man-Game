#pragma once

#include <iosfwd>

namespace pacman {

struct GameConfig {
    int initialLives{3};
    int frightenedDuration{40};  // în tick-uri
    int dotPoints{10};
    int powerPelletPoints{50};
    int ghostPoints{200};
    int scatterTicks{30};        // faza de retragere a fantomelor
    int chaseTicks{80};          // faza de urmărire
    int ghostSpeedDivider{2};    // fantomele se mișcă o dată la N tick-uri
    int tickMillis{120};         // durata unui tick în main

    GameConfig();                                              // implicit: valorile de mai sus
    GameConfig(int initialLives, int frightenedDuration, int dotPoints,
               int powerPelletPoints, int ghostPoints);        // cu parametri (restul rămân implicite)
    GameConfig(const GameConfig& other);                       // de copiere
    GameConfig& operator=(const GameConfig& other);            // de atribuire

    bool operator==(const GameConfig& other) const;
    bool operator!=(const GameConfig& other) const;

    bool isValid() const;
};

// Format:  GameConfig lives=3 frightened=40 dot=10 pellet=50 ghost=200 scatter=30 chase=80 divider=2 tick=120
std::ostream& operator<<(std::ostream& out, const GameConfig& config);
std::istream& operator>>(std::istream& in, GameConfig& config);

}  // namespace pacman
