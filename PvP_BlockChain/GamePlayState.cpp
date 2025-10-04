#include "GamePlayState.h"

GamePlayState::GamePlayState(sf::RenderWindow& win)
    : window(win), game(win), gameOver(false), winner(0)
{
    font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");
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
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                return StateID::Exit;
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
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
        window.clear(sf::Color::Red);
        game.render();

        sf::Text overText;
        overText.setFont(font);
        if (winner == 1)
            overText.setString("PLAYER 1 WINS!\nPress ESC to return");
        else if (winner == 2)
            overText.setString("PLAYER 2 WINS!\nPress ESC to return");
        else
            overText.setString("DRAW!\nPress ESC to return");

        overText.setFillColor(sf::Color::Black);
        overText.setCharacterSize(50);
        overText.setStyle(sf::Text::Bold);
        sf::FloatRect textRect = overText.getLocalBounds();
        overText.setOrigin(textRect.left + textRect.width / 2.0f,
            textRect.top + textRect.height / 2.0f);
        overText.setPosition(window.getSize().x / 2.0f, window.getSize().y / 2.0f);

        window.draw(overText);
        window.display();
    }
    else {
        window.clear();
        game.render();
        window.display();
    }
}
