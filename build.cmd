@echo off
rem Construire manuala a proiectului Pacman pe Windows (necesita g++ in PATH).
rem   build.cmd         - compileaza jocul in build\pacman.exe
rem   build.cmd run     - compileaza si porneste jocul
rem   build.cmd test    - compileaza si ruleaza testele
rem   build.cmd clean   - sterge folderul build
setlocal

set CXX=g++
set CXXFLAGS=-std=c++17 -Wall -Wextra -Wpedantic -O2 -Iinclude

if /I "%1"=="clean" goto clean
if not exist build mkdir build

for %%f in (Types GameEngine Renderer Listener) do (
    echo [CXX] src\%%f.cpp
    %CXX% %CXXFLAGS% -c src\%%f.cpp -o build\%%f.o
    if errorlevel 1 goto error
)

if /I "%1"=="test" goto test

echo [CXX] src\main.cpp
%CXX% %CXXFLAGS% -c src\main.cpp -o build\main.o
if errorlevel 1 goto error

echo [LD]  build\pacman.exe
%CXX% build\Types.o build\GameEngine.o build\Renderer.o build\Listener.o build\main.o -o build\pacman.exe
if errorlevel 1 goto error

echo Build reusit: build\pacman.exe
if /I "%1"=="run" build\pacman.exe
goto end

:test
echo [CXX] tests\smoke_test.cpp
%CXX% %CXXFLAGS% -c tests\smoke_test.cpp -o build\smoke_test.o
if errorlevel 1 goto error
%CXX% build\Types.o build\GameEngine.o build\Renderer.o build\Listener.o build\smoke_test.o -o build\smoke_test.exe
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
exit /b 1

:end
endlocal
