#include "MainMenuState.h"

MainMenuState::MainMenuState(sf::RenderWindow& win) : window(win) {
    font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");
    std::string labels[]{ "NEW GAME", "SETTINGS", "STORE", "EXIT" };
    for (int i = 0; i < 4; ++i) {
        sf::Text t;
        initText(t, labels[i], 350.f, 180.f + 75.f * i);
        buttons.push_back(t);
    }
}

void MainMenuState::initText(sf::Text& text, const std::string& str, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(40);
    text.setFillColor(sf::Color::White);
    text.setPosition(x, y);
}

StateID MainMenuState::update() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            return StateID::Exit;
        if (event.type == sf::Event::MouseButtonPressed) {
            auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (size_t i = 0; i < buttons.size(); ++i) {
                if (buttons[i].getGlobalBounds().contains(pos)) {
                    switch (i) {
                    case 0: return StateID::GamePlay;
                    case 1: return StateID::Settings;
                    case 2: return StateID::Store;
                    case 3: return StateID::Exit;
                    }
                }
            }
        }
    }
    return StateID::MainMenu;
}

void MainMenuState::render() {
    window.clear(sf::Color(50, 25, 16));
    for (auto& btn : buttons)
        window.draw(btn);
    window.display();
}
