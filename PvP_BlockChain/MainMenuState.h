#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include "Arena.h"

class MainMenuState {
public:
    MainMenuState(sf::RenderWindow& win);
    StateID update();
    void render();
private:
    sf::RenderWindow& window;
    sf::Font font;
    std::vector<sf::Text> buttons;
    Arena arena;
    void initText(sf::Text& text, const std::string& str, float x, float y);
};
