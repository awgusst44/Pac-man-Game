#pragma once

#include <memory>
#include <string>

#include "ScoreStorage.hpp"

namespace pacman {

// Păstrează scorurile într-o bază de date SQLite (bibliotecă externă: SQLite3).
// Headerul nu include <sqlite3.h>: detaliile bibliotecii sunt ascunse în implementare (pimpl),
// deci doar acest modul are nevoie de SQLite la compilare.
class SqliteScoreStorage : public ScoreStorage {
public:
    // Deschide (sau creează) baza de date și tabelul `scores`.
    // Calea ":memory:" creează o bază de date temporară, doar în memorie.
    explicit SqliteScoreStorage(const std::string& path);
    ~SqliteScoreStorage() override;

    SqliteScoreStorage(const SqliteScoreStorage&) = delete;
    SqliteScoreStorage& operator=(const SqliteScoreStorage&) = delete;

    bool isOpen() const;
    std::string lastError() const;  // ultimul mesaj de eroare SQLite (gol dacă nu a fost)

    // load: SELECT ... ORDER BY score DESC LIMIT <capacitatea tabelei>
    bool load(HighScores& scores) override;
    // save: într-o tranzacție, înlocuiește conținutul tabelei cu intrările date
    bool save(const HighScores& scores) override;
    std::string description() const override;

    // Versiunea bibliotecii SQLite folosite (ex. "3.45.1").
    static std::string libraryVersion();

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;
};

}  // namespace pacman
