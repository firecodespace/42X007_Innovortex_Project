#pragma once

#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

#include "Arena.h"
#include "Fighter.h"

class Game {
public:
    explicit Game(sf::RenderWindow& win);
    ~Game();

    void update(float dt);
    void render();
    void reset();
    bool isRunning() const;
    int getWinner() const;
    void setMapBackground(int mapIndex);
    void handleMouseClick(const sf::Vector2f& mousePos);

private:
    void triggerMintIfNeeded();
    static void copyToClipboard(const std::string& text);

    sf::RenderWindow& window;
    Arena arena;
    std::unique_ptr<Fighter> fighter1;
    std::unique_ptr<Fighter> fighter2;
    bool running = true;
    bool hit1ThisFrame = false;
    bool hit2ThisFrame = false;
    bool mintTriggered = false;
    bool awaitingConfirmation = false;
    bool mintSucceeded = false;
    sf::Music music;
    sf::Font statusFont;
    std::string statusTextString;
    std::string lastTransferTx;
    std::string lastMintTx;
    std::string lastTokenId;

    sf::RectangleShape confirmButton;
    sf::RectangleShape cancelButton;
    sf::RectangleShape copyTransferButton;
    sf::RectangleShape copyMintButton;
    sf::RectangleShape copyTokenButton;

    sf::Text confirmText;
    sf::Text cancelText;
    sf::Text copyTransferText;
    sf::Text copyMintText;
    sf::Text copyTokenText;
};
