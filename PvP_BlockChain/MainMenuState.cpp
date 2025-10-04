#include "MainMenuState.h"

MainMenuState::MainMenuState(sf::RenderWindow& win)
    : window(win),
    arena("Resources/Images/Video/ezgif-frame-001.jpg", win.getSize())
{
    font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");
    std::string labels[] = { "NEW GAME", "SETTINGS", "STORE", "EXIT" };
    float startX = 40.f;
    float startY = window.getSize().y - 240.f;
    float spacing = 60.f;
    for (int i = 0; i < 4; ++i) {
        sf::Text t;
        initText(t, labels[i], startX, startY + spacing * i);
        buttons.push_back(t);
    }
}

void MainMenuState::initText(sf::Text& text, const std::string& str, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(36);
    text.setFillColor(str == "NEW GAME" ? sf::Color(255, 215, 0) : sf::Color::White);
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
    window.clear();
    arena.draw(window);

    sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    sf::Text title("PIXEL PVP ARENA", font, 48);
    title.setFillColor(sf::Color::White);
    sf::FloatRect titleBound = title.getLocalBounds();
    title.setOrigin(titleBound.width / 2, titleBound.height / 2);
    title.setPosition(window.getSize().x / 2.f, 100.f);
    window.draw(title);

    for (auto& btn : buttons)
        window.draw(btn);

    window.display();
}
