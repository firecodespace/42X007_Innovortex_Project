#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include "Arena.h"

class MainMenuState : public State {  // FIXED: Added : public State
public:
    MainMenuState(sf::RenderWindow& win);
    StateID update(float dt) override;  // FIXED: Added float dt and override
    void render() override;  // FIXED: Added override
private:
    sf::RenderWindow& window;
    sf::Font font;
    std::vector<sf::Text> buttons;
    Arena arena;
    void initText(sf::Text& text, const std::string& str, float x, float y);
};
