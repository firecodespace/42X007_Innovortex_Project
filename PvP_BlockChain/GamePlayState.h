#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include "Game.h"

class GamePlayState {
public:
    GamePlayState(sf::RenderWindow& win);

    StateID update();
    void render();

private:
    sf::RenderWindow& window;
    Game game;
    sf::Clock clock;
};
