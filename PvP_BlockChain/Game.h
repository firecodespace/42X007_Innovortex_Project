#pragma once
#include <SFML/Graphics.hpp>
#include "Fighter.h"
#include "Arena.h"

class Game {
public:
    Game(sf::RenderWindow& window);
    void update(float dt);
    void render();
    bool isRunning() const;
    void reset();
    int getWinner() const;
private:
    sf::RenderWindow& window;
    Arena arena;
    Fighter* fighter1;
    Fighter* fighter2;
    bool running;
    bool hit1ThisFrame = false, hit2ThisFrame = false;
};
