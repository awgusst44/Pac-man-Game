# Pacman (C++)

## 1. Denumirea proiectului
**Pacman** — implementare în C++17 a jocului clasic, realizată ca proiect de laborator la disciplina *Programarea în limbajul C++ *.

## 2. Descrierea proiectului / reguli de joc
Jucătorul controlează personajul **Pacman** într-un labirint, mănâncă toate punctele (dots) și evită fantomele.

**Reguli:**
1. Pacman se deplasează continuu în direcția curentă; jucătorul poate schimba direcția cu tastele (W/A/S/D sau săgeți). Schimbarea se aplică imediat ce celula din noua direcție este liberă.
2. Pacman nu poate trece prin pereți (`Wall`). Marginile labirintului sunt tunel: ieșirea pe o parte aduce personajul pe partea opusă.
3. Fiecare punct (`Dot`) mâncat aduce **10 puncte**; fiecare bilă de putere (`PowerPellet`) aduce **50 de puncte**.
4. La mâncarea unei bile de putere, fantomele devin vulnerabile (`Frightened`) pentru un număr limitat de tick-uri. Mâncarea unei fantome vulnerabile aduce **200 de puncte**, iar fantoma se întoarce în casa fantomelor (`Eaten`).
5. Dacă Pacman atinge o fantomă nevulnerabilă, pierde o viață. Jucătorul începe cu **3 vieți**.
6. **Victorie:** toate punctele și bilele de putere au fost mâncate.
7. **Înfrângere:** viețile au ajuns la 0.
8. Comenzi suplimentare: `P` — pauză, `Q` — ieșire.

**Fantome (comportament):** Blinky urmărește direct, Pinky țintește în fața lui Pacman, Inky combină poziția lui Pacman cu a lui Blinky, Clyde alternează între urmărire și retragere. Fantomele alternează între modurile `Scatter` și `Chase`.

## 3. Structuri de date și descrierea lor
Toate tipurile se află în namespace-ul `pacman`.

| Structură / clasă | Fișier | Descriere |
|---|---|---|
| `Position` | `Types.hpp` | Coordonate `(x, y)` pe grila labirintului. |
| `Direction` | `Types.hpp` | Enum: `None`, `Up`, `Down`, `Left`, `Right`. |
| `CellType` | `Types.hpp` | Enum cu tipurile de celule: `Empty`, `Wall`, `Dot`, `PowerPellet`, `GhostHouse`. |
| `GameState` | `Types.hpp` | Enum cu starea jocului: `Menu`, `Running`, `Paused`, `Won`, `Lost`. |
| `GhostType`, `GhostMode` | `Types.hpp` | Tipul fantomei (Blinky, Pinky, Inky, Clyde) și modul ei (`Chase`, `Scatter`, `Frightened`, `Eaten`). |
| `Command` | `Types.hpp` | Enum cu acțiunile abstracte ale jucătorului (mișcare, pauză, ieșire), independent de dispozitivul de intrare. |
| `Pacman` | `Types.hpp` | Poziția, direcția curentă, direcția cerută și numărul de vieți ale jucătorului. |
| `Ghost` | `Types.hpp` | Tip, mod, poziție, poziție de start, direcție, timer de vulnerabilitate. |
| `Board` | `Types.hpp` | Grila labirintului: lățime, înălțime, `std::vector<CellType>`, acces `at(x, y)`, `inBounds(...)`. |
| `Score` | `Types.hpp` | Scorul curent și cel mai bun scor. |
| `GameConfig` | `Types.hpp` | Parametri: vieți inițiale, durata vulnerabilității, puncte per obiect. |
| `GameEngine` | `GameEngine.hpp` | **Motorul de joc:** ține starea, încarcă nivelul, procesează comenzi, actualizează Pacman și fantomele, verifică coliziunile. Nu știe nimic despre desenare sau tastatură. |
| `Renderer` / `ConsoleRenderer` | `Renderer.hpp` | **Desenatorul:** interfață abstractă care afișează starea motorului (doar citire). `ConsoleRenderer` este implementarea în consolă. |
| `Listener` / `ConsoleListener` | `Listener.hpp` | **Ascultătorul:** interfață abstractă care citește intrarea și o traduce în `Command`. `ConsoleListener` citește de la tastatură. |

**Arhitectura:** `Listener` → produce `Command` → `GameEngine` își actualizează starea → `Renderer` citește starea (`const`) și o afișează. Separarea celor trei componente permite înlocuirea consolei cu o interfață grafică (SDL, SFML) fără modificarea logicii jocului.

## Structura repozitoriului
```
include/
  Types.hpp
  GameEngine.hpp
  Renderer.hpp
  Listener.hpp
README.md
prompts_lab01.md
```
