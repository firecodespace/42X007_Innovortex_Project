#include "GamePlayState.h"
#include "MapSelectionState.h"

GamePlayState::GamePlayState(sf::RenderWindow& win)
    : window(win), game(win), font(), gameOver(false), winner(0), waitingForKeyRelease(false) {
    if (!font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF")) {
        
    }
    game.setMapBackground(MapSelectionState::getSelectedMap());
}

StateID GamePlayState::update(float dt) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            return StateID::Exit;

        if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::Enter) {
            waitingForKeyRelease = false;

            if (gameOver) {
                gameOver = false;
                winner = 0;
                game.reset();
                return StateID::MainMenu;
            }
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Enter) {
            waitingForKeyRelease = true;
        }
    }

    if (!gameOver) {
        game.update(dt);

        if (!game.isRunning()) {
            gameOver = true;
            winner = game.getWinner();
        }
    }

    return StateID::GamePlay;
}

void GamePlayState::render() {
    window.clear();
    game.render();

    if (gameOver) {
        sf::RectangleShape overlay(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
        overlay.setFillColor(sf::Color(0, 0, 0, 180));
        window.draw(overlay);

        std::string winnerText;
        if (winner == 1) winnerText = "PLAYER  1  WINS!";
        else if (winner == 2) winnerText = "PLAYER  2  WINS!";
        else winnerText = "DRAW!";

        sf::Text winText(winnerText, font, 60);
        winText.setFillColor(sf::Color::Yellow);
        winText.setOutlineColor(sf::Color::Black);
        winText.setOutlineThickness(3);
        sf::FloatRect bounds = winText.getLocalBounds();
        winText.setPosition((window.getSize().x - bounds.width) / 2.f,
            window.getSize().y / 2.f - 100.f);
        window.draw(winText);

        sf::Text instructions("PRESS  ENTER  TO  CONTINUE", font, 30);
        instructions.setFillColor(sf::Color::White);
        sf::FloatRect instBounds = instructions.getLocalBounds();
        instructions.setPosition((window.getSize().x - instBounds.width) / 2.f,
            window.getSize().y / 2.f + 50.f);
        window.draw(instructions);
    }

    window.display();
}
