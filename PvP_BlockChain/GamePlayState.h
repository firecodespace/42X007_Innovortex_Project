#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include "Game.h"

class GamePlayState {
public:
    GamePlayState(sf::RenderWindow& win);
    StateID update(float dt);  // <--- CHANGED THIS LINE - ADDED float dt
    void render();
private:
    sf::RenderWindow& window;
    Game game;
    sf::Clock clock;
    bool gameOver = false;
    int winner = 0;
    sf::Font font;
};
