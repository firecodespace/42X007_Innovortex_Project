#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
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
    sf::Music music;
    void initText(sf::Text& text, const std::string& str, float x, float y);

    // Wallet input
    std::string inputPlayer1;
    std::string inputPlayer2;
    bool inputActive1 = false;
    bool inputActive2 = false;
    sf::Text inputText1;
    sf::Text inputText2;

    // Optional gladiator character token IDs (digits) for on-chain recordVictory
    std::string inputCharToken1;
    std::string inputCharToken2;
    bool inputActiveChar1 = false;
    bool inputActiveChar2 = false;
    sf::Text inputTextChar1;
    sf::Text inputTextChar2;
};
