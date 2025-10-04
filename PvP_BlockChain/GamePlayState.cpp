#include "GamePlayState.h"

GamePlayState::GamePlayState(sf::RenderWindow& win)
    : window(win), game(win), font() {
    font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");
    gameOver = false;
    winner = 0;
}

StateID GamePlayState::update(float dt) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            return StateID::Exit;
    }

    game.update(dt);

    if (!game.isRunning()) {
        return StateID::MainMenu;
    }

    return StateID::GamePlay;
}

void GamePlayState::render() {
    window.clear();
    game.render();
    window.display();
}
