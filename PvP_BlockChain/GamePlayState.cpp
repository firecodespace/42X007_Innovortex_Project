#include "GamePlayState.h"
#include <windows.h>
#include <sstream>

GamePlayState::GamePlayState(sf::RenderWindow& win)
    : window(win), game(win) {
    font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");
}

GamePlayState::~GamePlayState() {
    if (mintThread.joinable()) {
        mintThread.join();
    }
}

StateID GamePlayState::update() {
    if (!gameOver) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return StateID::Exit;
        }
        float dt = clock.restart().asSeconds();
        game.update(dt);
        if (!game.isRunning()) {
            winner = game.getWinner();
            gameOver = true;
        }
        return StateID::GamePlay;
    }
    else {
        // Notify bridge once per finished match (background)
        if (!bridgeNotified) {
            bridgeNotified = true;
            if (winner == 1 || winner == 2) {
                std::string addr = WalletConfig::getPlayerWallet(winner);
                if (addr.empty()) {
                    // fallback to compile-time config
                    addr = (winner == 1) ? PLAYER1_WALLET : PLAYER2_WALLET;
                }
                // Launch background thread to avoid blocking the game loop
                mintInProgress = true;
                mintStatus = "Minting...";
                // determine loser address
                std::string loserAddr = (winner == 1) ? WalletConfig::getPlayerWallet(2) : WalletConfig::getPlayerWallet(1);
                if (loserAddr.empty()) {
                    loserAddr = (winner == 1) ? PLAYER2_WALLET : PLAYER1_WALLET;
                }

                mintThread = std::thread([this, addr, loserAddr]() {
                    std::string response;
                    bool ok = BridgeClient::postWin(addr, loserAddr, response);
                    std::stringstream ss;
                    if (ok) ss << "Mint OK: " << response;
                    else ss << "Mint failed: " << response;
                    {
                        std::lock_guard<std::mutex> lk(mintMutex);
                        mintStatus = ss.str();
                    }
                    mintInProgress = false;
                    // Also send a debug string
                    OutputDebugStringA(ss.str().c_str());
                });
            }
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return StateID::Exit;
            if (event.type == sf::Event::KeyPressed &&
                event.key.code == sf::Keyboard::Escape)
            {
                game.reset();
                gameOver = false;
                return StateID::MainMenu;
            }
        }
        return StateID::GamePlay;
    }
}

void GamePlayState::render() {
        if (gameOver) {
        window.clear(sf::Color::Red); // Red background when game over
        game.render();
        sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
        overlay.setFillColor(sf::Color(255, 0, 0, 150)); // Semi-transparent red overlay if desired
        window.draw(overlay);

        sf::Text overText;
        overText.setFont(font);
        if (winner == 1)
            overText.setString("PLAYER 1 WINS!\nPress ESC to return to menu");
        else if (winner == 2)
            overText.setString("PLAYER 2 WINS!\nPress ESC to return to menu");
        else
            overText.setString("DRAW!\nPress ESC to return to menu");
        overText.setFillColor(sf::Color::Black); // Black text
        overText.setCharacterSize(50);
        overText.setStyle(sf::Text::Bold);
        sf::FloatRect textRect = overText.getLocalBounds();
        overText.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        overText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

        window.draw(overText);
        // Draw mint status if present
        {
            std::lock_guard<std::mutex> lk(mintMutex);
            if (!mintStatus.empty()) {
                sf::Text msText(mintStatus, font, 20);
                msText.setFillColor(sf::Color::Black);
                msText.setPosition(20, 20);
                window.draw(msText);
            }
        }
        window.display();
    }
    else {
        // Normal rendering when game not over
        window.clear();
        game.render();
        window.display();
    }
}
