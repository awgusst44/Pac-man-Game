// Biblioteca pacman_sqlite: păstrarea scorurilor cu biblioteca externă SQLite3.
#include "SqliteScoreStorage.hpp"

#include <sqlite3.h>

#include <vector>

namespace pacman {

namespace {

// Resursele SQLite sunt eliberate automat: std::unique_ptr cu ștergător propriu.
using DatabasePtr = std::unique_ptr<sqlite3, int (*)(sqlite3*)>;
using StatementPtr = std::unique_ptr<sqlite3_stmt, int (*)(sqlite3_stmt*)>;

StatementPtr prepare(sqlite3* db, const std::string& sql) {
    sqlite3_stmt* raw = nullptr;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &raw, nullptr) != SQLITE_OK) {
        return StatementPtr(nullptr, &sqlite3_finalize);
    }
    return StatementPtr(raw, &sqlite3_finalize);
}

}  // namespace

struct SqliteScoreStorage::Impl {
    std::string path;
    std::string error;
    DatabasePtr db{nullptr, &sqlite3_close};

    explicit Impl(const std::string& databasePath) : path(databasePath) {
        sqlite3* raw = nullptr;
        const int result = sqlite3_open(path.c_str(), &raw);
        db.reset(raw);  // chiar și la eșec, SQLite întoarce un obiect care trebuie închis
        if (result != SQLITE_OK) {
            error = raw != nullptr ? sqlite3_errmsg(raw) : "memorie insuficienta";
            db.reset();
            return;
        }
        exec(
            "CREATE TABLE IF NOT EXISTS scores ("
            " id INTEGER PRIMARY KEY AUTOINCREMENT,"
            " name TEXT NOT NULL,"
            " score INTEGER NOT NULL,"
            " played_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP)");
    }

    bool exec(const std::string& sql) {
        char* message = nullptr;
        const int result = sqlite3_exec(db.get(), sql.c_str(), nullptr, nullptr, &message);
        if (result != SQLITE_OK) {
            error = message != nullptr ? message : sqlite3_errmsg(db.get());
            sqlite3_free(message);
            return false;
        }
        return true;
    }
};

SqliteScoreStorage::SqliteScoreStorage(const std::string& path) : impl_(std::make_unique<Impl>(path)) {}

SqliteScoreStorage::~SqliteScoreStorage() = default;

bool SqliteScoreStorage::isOpen() const { return impl_->db != nullptr; }

std::string SqliteScoreStorage::lastError() const { return impl_->error; }

std::string SqliteScoreStorage::description() const { return "SQLite: " + impl_->path; }

std::string SqliteScoreStorage::libraryVersion() { return sqlite3_libversion(); }

bool SqliteScoreStorage::load(HighScores& scores) {
    if (!isOpen()) {
        return false;
    }
    StatementPtr select =
        prepare(impl_->db.get(), "SELECT name, score FROM scores ORDER BY score DESC, name ASC LIMIT ?1");
    if (!select) {
        impl_->error = sqlite3_errmsg(impl_->db.get());
        return false;
    }
    sqlite3_bind_int(select.get(), 1, static_cast<int>(scores.capacity()));

    std::vector<ScoreEntry> rows;
    while (sqlite3_step(select.get()) == SQLITE_ROW) {
        const unsigned char* name = sqlite3_column_text(select.get(), 0);
        const int score = sqlite3_column_int(select.get(), 1);
        rows.emplace_back(name != nullptr ? reinterpret_cast<const char*>(name) : "", score);
    }
    scores.assign(std::move(rows));
    return true;
}

bool SqliteScoreStorage::save(const HighScores& scores) {
    if (!isOpen() || !impl_->exec("BEGIN TRANSACTION")) {
        return false;
    }
    bool ok = impl_->exec("DELETE FROM scores");
    if (ok) {
        StatementPtr insert = prepare(impl_->db.get(), "INSERT INTO scores(name, score) VALUES(?1, ?2)");
        ok = static_cast<bool>(insert);
        for (const ScoreEntry& entry : scores.entries()) {
            if (!ok) {
                break;
            }
            sqlite3_bind_text(insert.get(), 1, entry.name.c_str(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(insert.get(), 2, entry.score);
            ok = sqlite3_step(insert.get()) == SQLITE_DONE;
            sqlite3_reset(insert.get());
        }
        if (!ok) {
            impl_->error = sqlite3_errmsg(impl_->db.get());
        }
    }  // instrucțiunea este finalizată aici, înainte de COMMIT / ROLLBACK
    if (!impl_->exec(ok ? "COMMIT" : "ROLLBACK")) {
        ok = false;
    }
    return ok;
}

}  // namespace pacman
