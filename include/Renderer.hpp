#pragma once

#include "GameEngine.hpp"

namespace pacman {

class Renderer {
public:
    virtual ~Renderer() = default;

    virtual void init() = 0;
    virtual void render(const GameEngine& engine) = 0;
    virtual void shutdown() = 0;
};


class ConsoleRenderer : public Renderer {
public:
    void init() override;
    void render(const GameEngine& engine) override;
    void shutdown() override;

private:
    void clearScreen();
    char symbolFor(const GameEngine& engine, int x, int y) const;
    void drawHud(const GameEngine& engine);
};

}  