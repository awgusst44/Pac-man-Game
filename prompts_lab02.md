# Prompturi LLM — lab02 (Pacman, C++)

Am folosit Claude (Anthropic) ca asistent. Mai jos sunt interacțiunile, în ordine, cu rezultatul și ce a trebuit corectat sau lămurit.

## 1. Implementarea metodelor și construirea proiectului
**Prompt:** Am trimis enunțul laboratorului 02 (implementarea metodelor pentru fiecare structură declarată, compilarea unui fișier `.cpp` într-un fișier obiect din linia de comandă, fișier de construcție `cmd` / `Makefile` / `CMakeLists.txt`, `.gitignore` pentru fișierele obiect și executabile, actualizarea `README.md`, publicare în ramura `lab02`) și am cerut ajutor.

**Rezultat:**
- Headerele din lab01 au fost extinse cu metode, iar implementările au fost scrise în `src/`: `Types.cpp`, `GameEngine.cpp`, `Renderer.cpp`, `Listener.cpp`, `main.cpp`.
- Fișiere de construcție: `build.cmd` (Windows) și `Makefile` (Linux / MSYS2).
- `.gitignore` pentru `build/`, `*.o`, `*.exe`.
- `README.md` actualizat cu secțiunea „Construirea proiectului” și cu compilarea manuală a unui fișier obiect (`g++ -std=c++17 -Wall -Wextra -Iinclude -c src/Types.cpp -o build/Types.o`).
- Un fișier de teste `tests/smoke_test.cpp`.

**Verificare / greșeli:** codul a compilat fără avertismente (`-Wall -Wextra -Wpedantic`) și testele au trecut, inclusiv cu `-fsanitize=address,undefined`. Nu au fost necesare corecturi în cod.

## 2. Ce se păstrează din lab01
**Prompt:** „in lab02, ramine include si chestii din lab01?” și „hpp raman asa cum au fost?”.

**Rezultat:** lab02 se construiește peste lab01, deci folderul `include/` rămâne, dar headerele au fost extinse cu metode noi și trebuie înlocuite cu versiunile din lab02. Ramura `lab01` nu se modifică, pentru că este deja predată.

## 3. Rularea proiectului
**Prompt:** „cum se executa”.

**Rezultat:** pași pentru terminalul PowerShell din VS Code: `.\build.cmd run` (compilează și pornește jocul), `.\build.cmd test`, `.\build.cmd clean`, plus rezolvări pentru erorile frecvente (`g++` negăsit, caractere ciudate în loc de culori).

## 4. Problemă descoperită ulterior: `.gitignore` lipsea din proiect
**Context:** după ce am trecut pe ramura `lab03`, `git status` arăta fișiere din `build/` ca modificate (`GameEngine.o`, `Renderer.o`, `Types.o`). Fișierul `.gitignore` nu fusese copiat în proiect, iar fișierele compilate intraseră în Git.

**Rezolvare:** am creat `.gitignore` cu `build/`, `*.o`, `*.exe` și altele, apoi am scos folderul din urmărire cu `git rm -r --cached build` (fișierele rămân pe disc), am făcut commit și push. Am verificat și faptul că ramura `lab02` nu urmărește fișiere binare.

## Ce am modificat / ales eu
(Completează cu ce ai schimbat personal. Scrie doar ce ai făcut cu adevărat.)
-
