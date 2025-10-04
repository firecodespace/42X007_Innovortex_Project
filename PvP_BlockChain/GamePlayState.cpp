#include "GamePlayState.h"

GamePlayState::GamePlayState(sf::RenderWindow& win)
    : window(win), game(win) {
}

StateID GamePlayState::update() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            return StateID::Exit;
    }
    float dt = clock.restart().asSeconds();
    game.update(dt);
    if (!game.isRunning()) {
        game.reset(); // so next match is clean!
        return StateID::MainMenu;
    }
    return StateID::GamePlay;
}

void GamePlayState::render() {
    window.clear();
    game.render();
    window.display();
}
