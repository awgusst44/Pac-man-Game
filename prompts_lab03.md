# Prompturi LLM — lab03 (Pacman, C++)

Am folosit Claude (Anthropic) ca asistent. Mai jos sunt interacțiunile, în ordine, cu rezultatul și ce a trebuit corectat sau lămurit.

## 1. Constructori, operatori și clasa abstractă
**Prompt:** Am trimis enunțul laboratorului 03: pentru fiecare structură de joc, constructor implicit, de copiere și cu parametri, operator de atribuire, operatori de comparare și operatori de intrare/ieșire; pentru clasa care redă obiectele de joc, o clasă de bază abstractă. Am cerut să se construiască peste lab02.

**Rezultat:**
- `Types.hpp` / `Types.cpp`: pentru `Position`, `Pacman`, `Ghost`, `Board`, `Score` și `GameConfig` s-au definit constructorii (implicit, cu parametri, de copiere), atribuirea cu protecție la auto-atribuire, comparările și operatorii `<<` / `>>`.
- Comparările `< > <= >=` au fost definite doar pentru `Position` și `Score`, unde ordinea are sens; pentru restul `==` și `!=`.
- Operatorii `>>` citesc întâi într-un obiect temporar și modifică obiectul doar dacă citirea reușește, deci la format greșit obiectul rămâne neschimbat.
- `Board` are copie profundă.
- `Renderer` a devenit clasă abstractă (funcții virtuale pure, destructor virtual, fără copiere) cu metoda-șablon `render`; `ConsoleRenderer` este implementarea concretă.
- `main.cpp` a primit modul `--demo`, iar testele au fost extinse (inclusiv un `RecordingRenderer` care verifică ordinea apelurilor).

**Verificare / greșeli:** totul a compilat curat și testele au trecut din prima, inclusiv cu sanitizers. A fost verificat și că `Renderer r;` nu compilează.

## 2. Crearea ramurii lab03 păstrând lab02
**Prompt:** „cum creez lab03 pastrind fisierele din lab02”.

**Rezultat:** `git checkout lab02`, apoi `git checkout -b lab03`, copierea fișierelor modificate și `git push -u origin lab03`; ramurile `lab01` și `lab02` nu se modifică.

## 3. Fișiere `build/` apărute în `git status`
**Prompt:** am trimis rezultatul lui `git status` (fișierele `build/GameEngine.o`, `Renderer.o`, `Types.o` apăreau ca modificate) și apoi eroarea „nu se poate găsi calea” la `type .gitignore`.

**Problema:** `.gitignore` nu exista în proiect, deci fișierele compilate fuseseră adăugate în Git, iar un `.gitignore` creat ulterior nu afectează fișierele deja urmărite.

**Rezolvare:** am creat `.gitignore` (direct în editor, deoarece apărea gol), am șters fișierul `gitignore` creat din greșeală, fără punct, și am rulat `git rm -r --cached build`, urmat de commit și push. `git status` a arătat apoi „working tree clean”.

## 4. Verificarea finală
**Prompt:** „deci cu lab03 gata”, „cum sal deschid in powersell”.

**Rezultat:** listă de verificare înainte de predare (`.gitignore` completat, `build/` scos din urmărire, `.\build.cmd test` trece, ramura `lab03` publicată) și explicația că terminalul PowerShell din VS Code era deja deschis (``Ctrl+` ``).

## Ce am modificat / ales eu
(Completează cu ce ai schimbat personal. Scrie doar ce ai făcut cu adevărat.)
-
