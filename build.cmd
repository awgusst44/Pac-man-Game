@echo off
rem Construire manuala a proiectului Pacman pe Windows (necesita g++ si SQLite3).
rem
rem   build.cmd         - construieste bibliotecile statice si jocul (build\pacman.exe)
rem   build.cmd run     - construieste si porneste jocul
rem   build.cmd test    - construieste si ruleaza testele
rem   build.cmd clean   - sterge folderul build
rem
rem Biblioteca externa SQLite3 (o singura data, in terminalul "MSYS2 UCRT64"):
rem   pacman -S mingw-w64-ucrt-x86_64-sqlite3
setlocal

set CXX=g++
set AR=ar
set CXXFLAGS=-std=c++17 -Wall -Wextra -Wpedantic -O2 -Iinclude
set OBJ=build\obj
set LIB=build\lib

rem Bibliotecile se leaga intr-un grup (ordinea nu conteaza); SQLite3 la sfarsit.
set LINKLIBS=-L%LIB% -Wl,--start-group -lpacman_enums -lpacman_position -lpacman_pacman -lpacman_ghost -lpacman_board -lpacman_score -lpacman_config -lpacman_highscores -lpacman_sqlite -Wl,--end-group -lsqlite3
set APPOBJS=%OBJ%\GameEngine.o %OBJ%\Renderer.o %OBJ%\Listener.o %OBJ%\Game.o %OBJ%\Demo.o

if /I "%1"=="clean" goto clean
if not exist %OBJ% mkdir %OBJ%
if not exist %LIB% mkdir %LIB%

rem ---------- 1. Fisierele obiect ale bibliotecilor ----------
for %%f in (Enums Position Pacman Ghost Board Score GameConfig HighScores ScoreStorage SqliteScoreStorage) do (
    echo [CXX] src\%%f.cpp
    %CXX% %CXXFLAGS% -c src\%%f.cpp -o %OBJ%\%%f.o
    if errorlevel 1 goto error
)

rem ---------- 2. Bibliotecile statice: cate una pentru fiecare clasa de date ----------
echo [AR]  %LIB%\libpacman_enums.a
%AR% rcs %LIB%\libpacman_enums.a %OBJ%\Enums.o
if errorlevel 1 goto error

echo [AR]  %LIB%\libpacman_position.a
%AR% rcs %LIB%\libpacman_position.a %OBJ%\Position.o
if errorlevel 1 goto error

echo [AR]  %LIB%\libpacman_pacman.a
%AR% rcs %LIB%\libpacman_pacman.a %OBJ%\Pacman.o
if errorlevel 1 goto error

echo [AR]  %LIB%\libpacman_ghost.a
%AR% rcs %LIB%\libpacman_ghost.a %OBJ%\Ghost.o
if errorlevel 1 goto error

echo [AR]  %LIB%\libpacman_board.a
%AR% rcs %LIB%\libpacman_board.a %OBJ%\Board.o
if errorlevel 1 goto error

echo [AR]  %LIB%\libpacman_score.a
%AR% rcs %LIB%\libpacman_score.a %OBJ%\Score.o
if errorlevel 1 goto error

echo [AR]  %LIB%\libpacman_config.a
%AR% rcs %LIB%\libpacman_config.a %OBJ%\GameConfig.o
if errorlevel 1 goto error

echo [AR]  %LIB%\libpacman_highscores.a
%AR% rcs %LIB%\libpacman_highscores.a %OBJ%\HighScores.o %OBJ%\ScoreStorage.o
if errorlevel 1 goto error

echo [AR]  %LIB%\libpacman_sqlite.a
%AR% rcs %LIB%\libpacman_sqlite.a %OBJ%\SqliteScoreStorage.o
if errorlevel 1 goto error

rem ---------- 3. Aplicatia ----------
for %%f in (GameEngine Renderer Listener Game Demo) do (
    echo [CXX] src\%%f.cpp
    %CXX% %CXXFLAGS% -c src\%%f.cpp -o %OBJ%\%%f.o
    if errorlevel 1 goto error
)

if /I "%1"=="test" goto test

echo [CXX] src\main.cpp
%CXX% %CXXFLAGS% -c src\main.cpp -o %OBJ%\main.o
if errorlevel 1 goto error

echo [LD]  build\pacman.exe
%CXX% %APPOBJS% %OBJ%\main.o %LINKLIBS% -o build\pacman.exe
if errorlevel 1 goto error

echo Build reusit: build\pacman.exe
if /I "%1"=="run" build\pacman.exe
goto end

:test
echo [CXX] tests\smoke_test.cpp
%CXX% %CXXFLAGS% -c tests\smoke_test.cpp -o %OBJ%\smoke_test.o
if errorlevel 1 goto error
%CXX% %APPOBJS% %OBJ%\smoke_test.o %LINKLIBS% -o build\smoke_test.exe
if errorlevel 1 goto error
build\smoke_test.exe
goto end

:clean
if exist build rmdir /s /q build
echo Folderul build a fost sters.
goto end

:error
echo.
echo Compilarea a esuat.
echo Daca eroarea mentioneaza "sqlite3.h" sau "-lsqlite3", instaleaza biblioteca SQLite3
echo in terminalul "MSYS2 UCRT64":  pacman -S mingw-w64-ucrt-x86_64-sqlite3
exit /b 1

:end
endlocal
