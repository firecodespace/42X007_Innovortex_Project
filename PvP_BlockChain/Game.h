#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "BackgroundVideo.h"
#include "Fighter.h"
#include <iostream>

class Game {
public:
    Game(sf::RenderWindow& win);
    void reset();
    void update(float dt);
    void render();
    bool isRunning() const;
    int getWinner() const;

private:
    sf::RenderWindow& window;
    BackgroundVideo arena;
    Fighter* fighter1;
    Fighter* fighter2;
    bool running;
    bool hit1ThisFrame;
    bool hit2ThisFrame;
    sf::Music music;
};
