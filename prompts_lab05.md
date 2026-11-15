# Prompturi LLM — lab05 (Pacman, C++)

Am folosit Claude (Anthropic) ca asistent. Mai jos sunt interacțiunile, în ordine, cu rezultatul și ce a trebuit corectat sau lămurit.

## 1. Biblioteci proprii și bibliotecă externă
**Prompt:** Am trimis enunțul laboratorului 05: pentru fiecare clasă care reprezintă date să se creeze o bibliotecă de programare (statică sau dinamică, la alegere); să se folosească biblioteci externe pentru desenarea graficii sau pentru păstrarea datelor; fișierul de construire să fie rescris astfel încât aplicația să folosească bibliotecile. Am cerut să se construiască peste lab04.

**Rezultat:**
- **Biblioteci statice pentru clasele de date.** Vechiul `Types.hpp` / `Types.cpp` a fost împărțit în câte un header și o sursă pentru fiecare clasă: `Enums`, `Position`, `Pacman`, `Ghost`, `Board`, `Score`, `GameConfig`, plus `HighScores`. Fiecare devine o bibliotecă `libpacman_<nume>.a` (creată cu `ar rcs`). Am ales biblioteci statice pentru că rezultă un singur executabil, fără fișiere `.dll`.
- `Types.hpp` a rămas ca antet de comoditate care le include pe toate, iar ajutoarele pentru operatorii `>>` au trecut într-un header inline (`StreamParsing.hpp`), ca să nu aparțină niciunei biblioteci.
- **Bibliotecă externă: SQLite3 (păstrarea datelor).** Interfața abstractă `ScoreStorage` are două implementări: `TextScoreStorage` (fișier text) și `SqliteScoreStorage` (bază de date SQLite, în propria bibliotecă `libpacman_sqlite.a`). `Game` primește stocarea printr-un `std::unique_ptr<ScoreStorage>`. Scorurile se păstrează implicit în `highscores.db`; cu `--scores-file` se poate alege fișier text sau `.db`.
- `SqliteScoreStorage` folosește tabelul `scores`, `SELECT ... ORDER BY score DESC LIMIT ?`, tranzacții (`BEGIN`/`COMMIT`/`ROLLBACK`) și instrucțiuni pregătite. Resursele SQLite sunt păstrate în `std::unique_ptr` cu ștergător propriu, iar headerul nu include `<sqlite3.h>` (idiomul pimpl).
- **Fișiere de construire.** `build.cmd` și `Makefile` compilează clasele, creează bibliotecile statice, compilează aplicația și o leagă cu bibliotecile proprii și cu `-lsqlite3`. Bibliotecile se leagă într-un grup (`-Wl,--start-group ... --end-group`), ca ordinea lor să nu conteze.
- `.gitignore` a primit `*.db` și `*.sqlite`, README-ul a fost actualizat (secțiunile despre biblioteci și construire), iar testele au fost extinse (stocare text, SQLite în memorie cu `:memory:`, SQLite în fișier, cale invalidă, `Game` cu stocare SQLite).

## 2. Verificare și greșeli întâlnite
1. **SQLite în mediul de verificare:** în mediul în care a fost verificat codul nu exista headerul `sqlite3.h` și nu era acces la internet. Codul a fost testat cu motorul SQLite real, printr-un header minimal de compatibilitate folosit doar pentru testare (nu face parte din proiect). Pe calculatorul meu se folosește headerul oficial, instalat prin MSYS2 (`pacman -S mingw-w64-ucrt-x86_64-sqlite3`).
2. **Test cu așteptare greșită:** testul `Game` + SQLite a eșuat, pentru că numele „Sqlite Player” are 13 caractere, iar `sanitizeName` îl taie la 12. **Corectură:** numele din test a fost scurtat la „Sql Player” (rezultat `Sql_Player`).
3. **Verificări suplimentare:** simbolurile `sqlite3_*` apar doar în `libpacman_sqlite.a` (verificat cu `nm`), deci celelalte clase nu depind de SQLite. Baza de date scrisă de joc a fost citită și cu un program independent (Python), iar datele coincideau.

După corectură, codul a compilat fără avertismente (`-Wall -Wextra -Wpedantic`), iar testele au trecut, inclusiv cu `-fsanitize=address,undefined`.

## 3. Obținerea fișierelor
**Prompt:** „unde sunt fisierele pentru lab 5”.

**Rezultat:** proiectul a fost împachetat într-o arhivă `lab05.zip`, cu instrucțiuni de copiere peste proiect și de ștergere a fișierului vechi `src\Types.cpp`, care nu mai există în lab05.

## Ce am modificat / ales eu
(Completează cu ce ai făcut personal. Scrie doar ce ai făcut cu adevărat. De exemplu: că ai instalat SQLite, rezultatul rulării `.\build.cmd test`, ce ai schimbat în joc, ce ai decis la apărare.)
-
