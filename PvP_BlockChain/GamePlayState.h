#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include "Game.h"

class GamePlayState : public State {
public:
    GamePlayState(sf::RenderWindow& win);
    StateID update(float dt) override;
    void render() override;

private:
    sf::RenderWindow& window;
    Game game;
    sf::Font font;
    bool gameOver;
    int winner;
    bool waitingForKeyRelease; 
};
