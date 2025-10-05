#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>  // ADD THIS
#include "State.h"
#include "Arena.h"

class MainMenuState : public State {
public:
    MainMenuState(sf::RenderWindow& win);
    StateID update(float dt) override;
    void render() override;
private:
    sf::RenderWindow& window;
    sf::Font font;
    std::vector<sf::Text> buttons;
    Arena arena;
    sf::Music music;  // ADD THIS
    void initText(sf::Text& text, const std::string& str, float x, float y);

    // Wallet input
    std::string inputPlayer1;
    std::string inputPlayer2;
    bool inputActive1 = false;
    bool inputActive2 = false;
    sf::Text inputText1;
    sf::Text inputText2;
};
