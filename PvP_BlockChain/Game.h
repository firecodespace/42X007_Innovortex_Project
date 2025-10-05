#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "BackgroundVideo.h"
#include "Fighter.h"
#include <iostream>

#include <string>
#include <SFML/Graphics.hpp>

class Game {
public:
    Game(sf::RenderWindow& win);
    void setPlayerWallets(const std::string& p1, const std::string& p2) {
        player1Wallet = p1;
        player2Wallet = p2;
    }
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

    // status/UI for minting
    std::string statusTextString;
    sf::Font statusFont;

    // ensure we only trigger mint once per match
    bool mintTriggered = false;

    void triggerMintIfNeeded();
    void handleMouseClick(const sf::Vector2f& mousePos);

    // player wallets (strings like 0x...)
    std::string player1Wallet;
    std::string player2Wallet;
    // confirmation UI
    bool awaitingConfirmation = false;
    bool confirmed = false;
    sf::RectangleShape confirmButton;
    sf::RectangleShape cancelButton;
    sf::Text confirmText;
    sf::Text cancelText;
    // post-mint result UI
    bool mintSucceeded = false;
    std::string lastTransferTx;
    std::string lastMintTx;
    std::string lastTokenId;
    sf::RectangleShape copyTransferButton;
    sf::RectangleShape copyMintButton;
    sf::RectangleShape copyTokenButton;
    sf::Text copyTransferText;
    sf::Text copyMintText;
    sf::Text copyTokenText;
};
