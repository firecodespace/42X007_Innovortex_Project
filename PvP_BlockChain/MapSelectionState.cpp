#include "MapSelectionState.h"

int MapSelectionState::selectedMapIndex = 0;

MapSelectionState::MapSelectionState(sf::RenderWindow& win)
    : window(win),
    arena("Resources/Images/Video/ezgif-frame-001.jpg", win.getSize()) {
    font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");
    initMapButtons();
}

void MapSelectionState::initText(sf::Text& text, const std::string& str, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(32);
    text.setFillColor(sf::Color::White);
    text.setPosition(x, y);
}

void MapSelectionState::initMapButtons() {
    mapButtons.clear();
    std::string labels[] = { "ARENA MAP", "DESERT MAP", "FOREST MAP", "BACK TO MENU" };
    float startX = window.getSize().x / 2.f - 120;
    float startY = 180.f;
    float spacing = 60.f;

    for (int i = 0; i < 4; ++i) {
        sf::Text t;
        initText(t, labels[i], startX, startY + i * spacing);
        if (i == selectedMapIndex && i < 3) {
            t.setFillColor(sf::Color::Yellow);
        }
        mapButtons.push_back(t);
    }
}

StateID MapSelectionState::update(float dt) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            return StateID::Exit;

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            return StateID::MainMenu;
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (size_t i = 0; i < mapButtons.size(); ++i) {
                if (mapButtons[i].getGlobalBounds().contains(pos)) {
                    if (i < 3) {
                        selectedMapIndex = i;
                        return StateID::GamePlay;
                    }
                    else {
                        return StateID::MainMenu;
                    }
                }
            }
        }
    }
    return StateID::MapSelection;
}

void MapSelectionState::render() {
    window.clear();
    arena.draw(window);

    sf::RectangleShape overlay(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    sf::Text title("SELECT MAP", font, 48);
    title.setFillColor(sf::Color::White);
    sf::FloatRect titleBound = title.getLocalBounds();
    title.setOrigin(titleBound.width / 2, titleBound.height / 2);
    title.setPosition(window.getSize().x / 2.f, 100.f);
    window.draw(title);

    for (auto& btn : mapButtons)
        window.draw(btn);

    window.display();
}
