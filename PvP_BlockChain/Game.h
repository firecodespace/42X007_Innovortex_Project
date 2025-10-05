#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Fighter.h"
#include "Arena.h"

class Game {
public:
    Game(sf::RenderWindow& win);
    ~Game();  // ADD THIS LINE
    void update(float dt);
    void render();
    void reset();
    bool isRunning() const;
    int getWinner() const;
    void setMapBackground(int mapIndex);

private:
    sf::RenderWindow& window;
    Fighter player1;
    Fighter player2;
    Arena arena;
    bool running;
    int winner;
    bool hit1ThisFrame;
    bool hit2ThisFrame;
    sf::Music music;
};
