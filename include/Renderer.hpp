#pragma once

#include <string>
#include <vector>

#include "GameEngine.hpp"

namespace pacman {

// ======================================================================
//  Renderer — clasă de bază ABSTRACTĂ pentru redarea obiectelor de joc.
//
//  Nu poate fi instanțiată: conține funcții virtuale pure, pe care fiecare
//  desenator concret (consolă, SDL, SFML, un test) trebuie să le implementeze.
//  render() este metoda-șablon: fixează ordinea desenării, iar clasele derivate
//  decid doar CUM se desenează fiecare element.
// ======================================================================
class Renderer {
public:
    Renderer() = default;
    virtual ~Renderer() = default;  // destructor virtual: ștergere sigură prin Renderer*

    // Un desenator este o resursă (ecran, fereastră): nu se copiază.
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;

    virtual void init() = 0;
    virtual void shutdown() = 0;

    // Desenează un cadru complet din starea motorului (doar citire).
    void render(const GameEngine& engine);

protected:
    virtual void beginFrame() = 0;
    virtual void drawBoard(const Board& board) = 0;
    virtual void drawPacman(const Pacman& pacman) = 0;
    virtual void drawGhost(const Ghost& ghost) = 0;
    virtual void drawHud(const Score& score, int lives, int dotsRemaining) = 0;
    virtual void drawStatus(GameState state, const Score& score) = 0;
    virtual void endFrame() = 0;
};

// Implementare concretă: desenează în consolă (ASCII + culori ANSI).
class ConsoleRenderer : public Renderer {
public:
    void init() override;
    void shutdown() override;

private:
    void beginFrame() override;
    void drawBoard(const Board& board) override;
    void drawPacman(const Pacman& pacman) override;
    void drawGhost(const Ghost& ghost) override;
    void drawHud(const Score& score, int lives, int dotsRemaining) override;
    void drawStatus(GameState state, const Score& score) override;
    void endFrame() override;

    void clearScreen();
    void put(const Position& position, const std::string& symbol);

    int width_{0};
    int height_{0};
    std::vector<std::string> cells_;  // simbolul (cu culoare) pentru fiecare celulă
    std::string hud_;
    std::string status_;
};

}  // namespace pacman
