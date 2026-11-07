# Prompturi LLM — lab04 (Pacman, C++)

Am folosit Claude (Anthropic) ca asistent. Mai jos sunt interacțiunile, în ordine, cu rezultatul și ce a trebuit corectat sau lămurit.

## 1. Rescrierea cu STL
**Prompt:** Am trimis enunțul laboratorului 04: rescrierea codului astfel încât să se folosească containere STL în loc de vectori (tablouri), fluxuri de intrare/ieșire pentru citirea și afișarea datelor, referințe și smart pointers în loc de pointeri și alocare dinamică, `std::string` în loc de șiruri de caractere și algoritmi STL pentru căutare, sortare, comparare și copiere. Am cerut să se construiască peste lab03.

**Rezultat:**
- **Containere:** `std::array` pentru direcții, `std::map` pentru tastele din `ConsoleListener` și pentru parcurgerea fantomelor mâncate, `std::set` și `std::queue` pentru parcurgerea în lățime și validarea nivelului, `std::optional` în locul pointerilor nuli.
- **Fluxuri:** nivelul se citește dintr-un fișier (`levels/level1.txt`) cu `ifstream`; tabela de scoruri se citește și se scrie cu `ifstream`, `ofstream`, `istream_iterator` și `ostream_iterator`; `operator<<` pentru `GameEngine`.
- **Smart pointers:** noua clasă `Game` deține `Renderer` și `Listener` prin `std::unique_ptr`, iar `GameConfig` este partajat cu motorul prin `std::shared_ptr`; fără `new` / `delete`.
- **`std::string`:** `toString` întoarce `std::string`, constantele de culoare și parametrii funcțiilor de citire sunt `std::string`.
- **Algoritmi:** `std::sort` (scoruri), `std::find_if` și `std::min_element` (căutare, alegerea direcției fantomei), `std::equal` și `std::includes` (comparare), `std::copy`, `std::copy_if` și `std::set_union` (copiere).
- **Funcții noi:** tabelă de scoruri (`HighScores`), opțiunile `--level`, `--scores`, `--name`, `--demo`, fișierul `levels/level1.txt`.
- Fișiere noi: `HighScores`, `Game`, `Demo`; `Makefile` și `build.cmd` au fost actualizate.

## 2. Greșeli întâlnite la verificare și corecturi
1. **Eroare de compilare într-un test (vexing parse):** linia `GameEngine fallback(std::shared_ptr<const GameConfig>(invalid));` a fost interpretată de compilator ca o declarație de funcție. **Corectură:** obiectul `shared_ptr` a fost pus într-o variabilă separată, apoi dat motorului.
2. **Lipsă `#include <istream>`** în `GameEngine.cpp` (headerul declara doar `<iosfwd>`). **Corectură:** s-a adăugat includerea.
3. **Așteptare greșită într-un test:** nivelul de test avea doar 3 puncte, deci jocul se termina cu victorie înainte de comanda de ieșire și scorul nu mai era înregistrat de `finish()`. **Corectură:** nivelul a fost lungit (6 puncte) și valoarea așteptată a fost corectată la 30 (3 pași × 10 puncte).
4. **Citire blocantă pe Linux:** la rularea cu intrarea dată printr-o conductă, `read()` bloca bucla jocului. **Corectură:** citirea folosește `select()` și nu mai blochează (partea de Windows, `_kbhit` / `_getch`, nu a fost modificată).
5. **README:** a fost clarificat faptul că „vectori” a fost interpretat ca tablouri clasice (C), iar `std::vector` se păstrează acolo unde este alegerea potrivită.

După corecturi, codul a compilat fără avertismente și toate testele au trecut, inclusiv cu `-fsanitize=address,undefined`.

## 3. Eroare la rularea pe Windows
**Prompt:** am trimis o captură a terminalului din VS Code: `build.cmd` se oprea cu „tests\smoke_test.cpp: No such file or directory”.

**Problema:** fișierul `smoke_test.cpp` fusese pus în `src/` în loc de `tests/`, iar `level1.txt` era în rădăcina proiectului în loc de `levels/`; mai exista și un fișier `gitignore` fără punct, creat din greșeală.

**Rezolvare:** am creat folderele `tests/` și `levels/` și am mutat fișierele (`move src\smoke_test.cpp tests\`, `move level1.txt levels\`), am șters `gitignore` și am adăugat `highscores.txt` și `*.tmp` în `.gitignore`, ca să nu fie urmărite. După aceea `.\build.cmd test` și `.\build.cmd run` au funcționat.

## Ce am modificat / ales eu
(Completează cu ce ai schimbat personal. Scrie doar ce ai făcut cu adevărat.)
-
