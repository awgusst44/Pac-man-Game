#pragma once

#include <iosfwd>
#include <string>

#include "GameEngine.hpp"

namespace pacman {

// Desenatorul: afișează starea motorului. Nu modifică jocul.
class Renderer {
public:
    virtual ~Renderer() = default;

    virtual void init() = 0;
    virtual void render(const GameEngine& engine) = 0;
    virtual void shutdown() = 0;
};

// Implementare în consolă (ASCII + culori ANSI).
class ConsoleRenderer : public Renderer {
public:
    void init() override;
    void render(const GameEngine& engine) override;
    void shutdown() override;

private:
    void clearScreen();
    std::string symbolFor(const GameEngine& engine, int x, int y) const;
    void drawHud(const GameEngine& engine, std::ostream& out) const;
    void drawFooter(const GameEngine& engine, std::ostream& out) const;
};

}  // namespace pacman
