#include "Game.h"

Game::Game(sf::RenderWindow& win)
    : window(win),
    arena("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/Background/arena.jpg", window.getSize()),
    fighter1(nullptr), fighter2(nullptr), running(true)
{
    reset();
}

void Game::reset() {
    if (fighter1) delete fighter1;
    if (fighter2) delete fighter2;
    fighter1 = new Fighter(
        200.f, window.getSize().y - 128.f * 2.0f - 12,
        sf::Keyboard::A, sf::Keyboard::D,
        sf::Keyboard::F, sf::Keyboard::G, sf::Keyboard::H,
        sf::Keyboard::W, sf::Keyboard::Q);

    fighter2 = new Fighter(
        700.f, window.getSize().y - 128.f * 2.0f - 12,
        sf::Keyboard::Left, sf::Keyboard::Right,
        sf::Keyboard::Numpad1, sf::Keyboard::Numpad2, sf::Keyboard::Numpad3,
        sf::Keyboard::Up, sf::Keyboard::RControl);
    running = true;
}

void Game::update(float dt) {
    fighter1->update(dt, window);
    fighter2->update(dt, window);

    // ESC returns to menu
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        running = false;

    // -- Insert your win/lose logic here, for example:
    // if (fighter1->getHP() <= 0 || fighter2->getHP() <= 0)
    //     running = false;
}

void Game::render() {
    arena.draw(window);
    fighter1->draw(window);
    fighter2->draw(window);
    fighter1->drawHealthBar(window, true);
    fighter2->drawHealthBar(window, false);
}

bool Game::isRunning() const {
    return running;
}
