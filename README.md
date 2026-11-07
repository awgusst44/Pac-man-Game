# Pacman (C++)

## 1. Denumirea proiectului
**Pacman** — implementare în C++17 a jocului clasic, realizată ca proiect de laborator la disciplina *Metaprogramare (C++)*. Jocul rulează în consolă.

## 2. Descrierea proiectului / reguli de joc
Jucătorul controlează personajul **Pacman** într-un labirint, mănâncă toate punctele (dots) și evită fantomele.

**Reguli:**
1. Pacman se deplasează continuu în direcția curentă; jucătorul poate schimba direcția cu tastele. Virajul cerut se aplică imediat ce celula din noua direcție este liberă.
2. Pacman nu poate trece prin pereți (`Wall`) și nu poate intra în casa fantomelor. Pe rândurile cu marginile deschise, ieșirea pe o parte aduce personajul pe partea opusă (tunel).
3. Fiecare punct (`Dot`) mâncat aduce **10 puncte**; fiecare bilă de putere (`PowerPellet`) aduce **50 de puncte**.
4. La mâncarea unei bile de putere, fantomele devin vulnerabile (`Frightened`) pentru un număr limitat de tick-uri. Mâncarea unei fantome vulnerabile aduce **200 de puncte**, iar fantoma se întoarce pe drumul cel mai scurt în casa fantomelor (`Eaten`).
5. Dacă Pacman atinge o fantomă nevulnerabilă, pierde o viață, iar Pacman și fantomele revin la pozițiile de start. Jucătorul începe cu **3 vieți**.
6. **Victorie:** toate punctele și bilele de putere au fost mâncate.
7. **Înfrângere:** viețile au ajuns la 0.
8. După victorie sau înfrângere, o tastă de mișcare pornește un joc nou (recordul se păstrează).

**Comenzi:** `W` `A` `S` `D` sau săgețile — mișcare, `P` — pauză, `Q` — ieșire.

**Fantome (comportament):** fantomele alternează între `Scatter` (se duc spre colțul lor) și `Chase` (urmăresc): *Blinky* țintește direct Pacman, *Pinky* țintește 4 celule în fața lui, *Inky* folosește poziția lui Blinky și a lui Pacman, *Clyde* urmărește de la distanță și se retrage când este aproape. Fantomele nu se întorc pe loc (cu excepția fundurilor de sac) și nu mai intră în casa lor decât după ce sunt mâncate.

## 3. Structuri de date și descrierea lor
Toate tipurile se află în namespace-ul `pacman`. Declarațiile sunt în `include/`, implementările în `src/`.

| Structură / clasă | Fișiere | Descriere |
|---|---|---|
| `Position` | `Types` | Coordonate `(x, y)`; `moved`, `distanceSquared`; ordonată, deci poate fi cheie în `std::set` / `std::map`. |
| `Direction`, `CellType`, `GameState`, `GhostType`, `GhostMode`, `Command` | `Types` | Enum-uri: direcții, celule, starea jocului, fantome, comenzi abstracte. |
| `Pacman`, `Ghost` | `Types` | Personajele jocului (poziție, direcție, vieți / mod, timer de vulnerabilitate). |
| `Board` | `Types` | Grila labirintului (`std::vector<CellType>`); `wrap` (tunel), `isWalkable`, `positionsOf` (`std::set`), `reachableFrom` (parcurgere în lățime). |
| `Score`, `GameConfig` | `Types` | Scorul curent / recordul și parametrii jocului. |
| `GameEngine` | `GameEngine` | **Motorul de joc:** încarcă nivelul (din vector de rânduri sau din flux), procesează comenzi, mută Pacman și fantomele, verifică coliziunile. Nu știe nimic despre desenare sau tastatură. |
| `Renderer` *(abstractă)* | `Renderer` | Clasa de bază abstractă a desenatorului (secțiunea 4.4). |
| `ConsoleRenderer` | `Renderer` | Implementarea concretă: desenează în consolă cu culori ANSI. |
| `Listener` / `ConsoleListener` | `Listener` | **Ascultătorul:** interfață abstractă; `ConsoleListener` citește tastatura fără blocare, folosind tabele `std::map` pentru taste. |
| `ScoreEntry`, `HighScores` | `HighScores` | **Nou (lab04).** Tabela celor mai bune scoruri: sortare, căutare, citire/scriere prin fișiere. |
| `Game` | `Game` | **Nou (lab04).** Aplicația: deține `Renderer` și `Listener` prin `std::unique_ptr`, partajează configurația prin `std::shared_ptr`, ține tabela de scoruri. |
| `runDemo` | `Demo` | **Nou (lab04).** Demonstrația `--demo`. |

**Arhitectura:** `Listener` → produce `Command` → `GameEngine` își actualizează starea → `Renderer` citește starea (`const`) și o afișează. `Game` leagă cele trei componente și înregistrează scorul la sfârșitul jocului.

**Nivelul** este un fișier text, `levels/level1.txt`: `#` perete, `.` punct, `o` bilă de putere, `P` Pacman, `G` fantomă (în casa fantomelor), spațiu = gol. Rândurile care încep cu `;` sunt comentarii, iar rândurile mai scurte se completează cu spații. Dacă fișierul lipsește, se folosește nivelul încorporat (`Game::defaultLevel()`). Un nivel în care nu toate punctele pot fi mâncate este respins la încărcare.

## 4. Constructori, operatori și clasa abstractă (lab03)

### 4.1. Ce are fiecare structură de joc
Fiecare structură are definite explicit (în `Types.cpp`): constructor implicit, constructor cu parametri, constructor de copiere, operator de atribuire (cu protecție la auto-atribuire), operatori de comparare și operatori `<<` / `>>`.

| Structură | Constructor implicit | Constructor cu parametri | Comparare |
|---|---|---|---|
| `Position` | `(0, 0)` | `Position(x, y)` | `== != < > <= >=` (lexicografic: x, apoi y) |
| `Pacman` | 3 vieți, poziția `(0, 0)` | `Pacman(spawn, lives)` | `== !=` |
| `Ghost` | Blinky, mod `Scatter`, `(0, 0)` | `Ghost(type, spawn)` | `== !=` |
| `Board` | grilă goală `0 x 0` | `Board(width, height, fill)` | `== !=` |
| `Score` | `0 / 0` | `Score(current, best)` | `== != < > <= >=` (scorul curent, apoi recordul) |
| `GameConfig` | valorile implicite ale jocului | `GameConfig(lives, frightened, dot, pellet, ghost)` | `== !=` |

Observații:
- **Constructorul de copiere și atribuirea** copiază toate câmpurile. La `Board` copia este *profundă*: `std::vector` își copiază elementele, deci două grile nu împart memorie (modificarea copiei nu afectează originalul).
- **Comparare:** relațiile de ordine (`<`, `>`, `<=`, `>=`) sunt definite doar acolo unde ordinea are sens (`Position`, `Score`). Pentru `Pacman`, `Ghost`, `Board` și `GameConfig` ordinea nu are o semnificație naturală, deci sunt definite `==` și `!=`.
- **Validare:** constructorii cu parametri corectează valorile imposibile (viețile negative devin 0, dimensiunile negative ale grilei devin 0, recordul nu poate fi mai mic decât scorul curent).

### 4.2. Operatorii de intrare / ieșire
Operatorul `<<` scrie obiectul text, iar `>>` îl citește înapoi, deci `scrie → citește` dă un obiect egal cu originalul. La un format greșit, `>>` setează `failbit` și **nu modifică** obiectul.

| Tip | Format |
|---|---|
| `Position` | `(3,4)` |
| `Pacman` | `Pacman pos=(1,2) spawn=(1,2) dir=Left next=Up lives=3` |
| `Ghost` | `Ghost type=Blinky mode=Scatter pos=(1,2) spawn=(1,2) dir=None frightened=0` |
| `Score` | `Score current=10 best=20` |
| `GameConfig` | `GameConfig lives=3 frightened=40 dot=10 pellet=50 ghost=200 scatter=30 chase=80 divider=2 tick=120` |
| `Board` | linia `Board 5x3`, apoi 3 rânduri de câte 5 caractere (`#` perete, `.` punct, `o` bilă, `-` casa fantomelor, spațiu gol) |
| enum-uri | numele valorii (`Left`, `Inky`, `Frightened`); `CellType` se scrie ca simbol |

Exemplu:
```cpp
std::stringstream stream;
stream << Pacman(Position(10, 11), 2);   // ieșire
Pacman restored;
stream >> restored;                       // intrare
```

### 4.3. Demonstrație
Programul are un mod de demonstrație, care arată constructorii, atribuirea, comparările și operatorii de intrare/ieșire:
```
build\pacman.exe --demo
```

### 4.4. Clasa de bază abstractă `Renderer`
`Renderer` este responsabilă de redarea obiectelor de joc și este **abstractă**: are funcții virtuale pure, deci nu poate fi instanțiată (`Renderer r;` nu compilează).

- **Funcții virtuale pure:** `init`, `shutdown`, `beginFrame`, `drawBoard`, `drawPacman`, `drawGhost`, `drawHud`, `drawStatus`, `endFrame`.
- **Metoda-șablon** `render(const GameEngine&)` (nevirtuală) fixează ordinea: tabla, fantomele, Pacman (deasupra), HUD, mesajul de stare. Clasele derivate decid doar *cum* se desenează fiecare element.
- **Destructor virtual**, pentru ștergere sigură printr-un `Renderer*`.
- **Fără copiere** (constructorul de copiere și atribuirea sunt șterse): un desenator reprezintă o resursă (ecran, fereastră).
- `ConsoleRenderer` este implementarea concretă. Pentru o altă interfață (SDL, SFML) se scrie o nouă clasă derivată, fără a modifica motorul sau bucla de joc. `main.cpp` lucrează prin `Renderer&`, iar testele folosesc un `RecordingRenderer` care verifică ordinea apelurilor.

## 5. Rescrierea cu STL (lab04)

> **Notă:** prin „vectori" se înțeleg tablourile clasice din C (`T v[N]`, `char*`). În cod nu mai există tablouri C, `new`/`delete`, pointeri expuși sau `char*`. `std::vector` rămâne acolo unde este alegerea potrivită (secvență dinamică), iar în rest se folosesc alte containere STL.

### 5.1. Containere STL
| Container | Unde este folosit | De ce |
|---|---|---|
| `std::vector` | grila `Board::cells`, fantomele, rândurile nivelului, intrările din `HighScores` | secvență dinamică cu acces rapid după index |
| `std::array<Direction,4>` | cele 4 direcții (motor, parcurgerea în lățime) | dimensiune fixă, cunoscută la compilare |
| `std::map<int,Command>` | tastele și săgețile din `ConsoleListener` | tabel de asociere tastă → comandă, în locul unui `switch` |
| `std::map<Position,Direction>` | parcurgerea fantomelor mâncate (BFS) | pentru fiecare celulă vizitată, prima direcție spre ea |
| `std::set<Position>` | `Board::positionsOf`, `Board::reachableFrom`, validarea nivelului | mulțimi ordonate, fără duplicate |
| `std::queue<Position>` | parcurgerile în lățime | coadă FIFO |
| `std::optional<ScoreEntry>` | `HighScores::best()` | „poate nu există" fără pointer nul |

### 5.2. Fluxuri de intrare / ieșire
- **Fișiere:** `std::ifstream` citește nivelul (`levels/level1.txt`) și tabela de scoruri; `std::ofstream` salvează scorurile (`highscores.txt`).
- **Șiruri ca fluxuri:** `std::istringstream` / `std::ostringstream` în teste și în demonstrație.
- **Operatori:** `operator<<` / `operator>>` pentru toate structurile (lab03), pentru `ScoreEntry` și `operator<<` pentru `GameEngine` (scrie nivelul în formatul din fișier).
- **Iteratori de flux:** `std::istream_iterator<ScoreEntry>` citește tabela, `std::ostream_iterator<ScoreEntry>` o scrie.
- **Formatare:** `std::setw`, `std::left`, `std::right` pentru tabelul de scoruri.

### 5.3. Referințe și smart pointers
| Element | Ce folosește |
|---|---|
| `Game::renderer_`, `Game::listener_` | `std::unique_ptr<Renderer>` / `std::unique_ptr<Listener>`: proprietate unică, ștergere automată prin destructorul virtual |
| `GameConfig` | `std::shared_ptr<const GameConfig>`: același obiect partajat între `Game` și `GameEngine`, fără copii |
| `std::make_unique`, `std::make_shared` | crearea obiectelor, fără `new` / `delete` |
| funcții și metode | parametri `const T&` (de exemplu `const Board&`, `const Position&`) și `Renderer&` în loc de pointeri |
| `std::optional` | în locul pointerilor nuli |

### 5.4. `std::string`
`toString(...)` întoarce `std::string`, constantele de culoare ANSI din `Renderer.cpp` sunt `std::string`, funcțiile de citire primesc `const std::string&`, iar argumentele programului se convertesc în `std::vector<std::string>`.

### 5.5. Algoritmi STL
| Operație | Algoritmi | Unde |
|---|---|---|
| **Căutare** | `std::find`, `std::find_if`, `std::find_if_not`, `std::any_of`, `std::all_of`, `std::count`, `std::min_element`, `std::max_element` | alegerea direcției fantomei (`min_element`), căutarea lui Blinky pentru Inky, locul unui jucător, validarea rândurilor nivelului |
| **Sortare** | `std::sort` | `HighScores::normalize` (scor descrescător, apoi nume) |
| **Comparare** | `std::equal`, `std::includes`, comparare lexicografică cu `std::tie` | `Board::operator==`, `HighScores::operator==`, validarea nivelului (`food ⊆ reachable`), `Position::operator<` |
| **Copiere** | `std::copy`, `std::copy_if`, `std::fill`, `std::set_union`, `std::remove_if` | copia profundă a `Board`, filtrarea direcțiilor posibile, scrierea/citirea scorurilor, reuniunea punctelor și bilelor |
| **Altele** | `std::for_each`, `std::replace`, `std::accumulate` | speriarea / resetarea fantomelor, numele jucătorului, suma scorurilor |

### 5.6. Funcții noi
- **Nivel din fișier:** `build\pacman.exe --level levels\level1.txt` (implicit se încearcă `levels/level1.txt`).
- **Tabela de scoruri:** scorul se înregistrează la victorie, la înfrângere sau la ieșirea cu `Q`; se salvează în `highscores.txt` (ignorat de Git). `build\pacman.exe --scores` afișează tabela, iar `--name <nume>` setează numele jucătorului.
- **Demonstrație:** `build\pacman.exe --demo` arată containerele, algoritmii, fluxurile și smart pointers.
- **Ajutor:** `build\pacman.exe --help`.

## 6. Construirea proiectului
Este necesar un compilator C++17 (`g++`, de exemplu MinGW-w64 din MSYS2). Proiectul nu folosește biblioteci externe.

### Windows (cmd / PowerShell) — `build.cmd`
```
build.cmd          :: compilează jocul în build\pacman.exe
build.cmd run      :: compilează și pornește jocul
build.cmd test     :: compilează și rulează testele
build.cmd clean    :: șterge folderul build
```
Rulează jocul într-un terminal modern (Windows Terminal sau terminalul din VS Code; culorile folosesc coduri ANSI).

### Linux sau terminal MSYS2 — `Makefile`
```
make          # compilează jocul în build/pacman
make run      # compilează și pornește jocul
make test     # compilează și rulează testele
make clean    # șterge folderul build
```

### Compilare manuală, pas cu pas
```
mkdir build
g++ -std=c++17 -Wall -Wextra -Iinclude -c src/Types.cpp -o build/Types.o
g++ -std=c++17 -Wall -Wextra -Iinclude -c src/GameEngine.cpp -o build/GameEngine.o
g++ -std=c++17 -Wall -Wextra -Iinclude -c src/Renderer.cpp -o build/Renderer.o
g++ -std=c++17 -Wall -Wextra -Iinclude -c src/Listener.cpp -o build/Listener.o
g++ -std=c++17 -Wall -Wextra -Iinclude -c src/HighScores.cpp -o build/HighScores.o
g++ -std=c++17 -Wall -Wextra -Iinclude -c src/Game.cpp -o build/Game.o
g++ -std=c++17 -Wall -Wextra -Iinclude -c src/Demo.cpp -o build/Demo.o
g++ -std=c++17 -Wall -Wextra -Iinclude -c src/main.cpp -o build/main.o
g++ build/Types.o build/GameEngine.o build/Renderer.o build/Listener.o build/HighScores.o build/Game.o build/Demo.o build/main.o -o build/pacman
```
Opțiuni: `-std=c++17` — standardul limbajului, `-Wall -Wextra` — avertismente, `-Iinclude` — folderul cu headere, `-c` — doar compilare (fișier obiect), `-o` — numele fișierului rezultat.

Fișierele obiect și executabilele sunt ignorate de Git (`.gitignore`).

## Structura repozitoriului
```
include/            Types.hpp  GameEngine.hpp  Renderer.hpp  Listener.hpp
                    HighScores.hpp  Game.hpp  Demo.hpp
src/                Types.cpp  GameEngine.cpp  Renderer.cpp  Listener.cpp
                    HighScores.cpp  Game.cpp  Demo.cpp  main.cpp
tests/              smoke_test.cpp
levels/             level1.txt
build.cmd           construire pe Windows
Makefile            construire pe Linux / MSYS2
.gitignore          ignoră build/, *.o, *.exe, highscores.txt
prompts_lab01.md    prompturile LLM folosite
README.md
```
