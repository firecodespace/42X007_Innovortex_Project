#include "Game.h"
#include <iostream>

Game::Game(sf::RenderWindow& win)
    : window(win),
    arena("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/Background/arena.jpg", win.getSize()),
    player1(200.f, win.getSize().y - 128.f * 2.0f - 12,
        sf::Keyboard::A, sf::Keyboard::D,
        sf::Keyboard::F, sf::Keyboard::G, sf::Keyboard::H,
        sf::Keyboard::W, sf::Keyboard::Q),
    player2(700.f, win.getSize().y - 128.f * 2.0f - 12,
        sf::Keyboard::Left, sf::Keyboard::Right,
        sf::Keyboard::Numpad1, sf::Keyboard::Numpad2, sf::Keyboard::Numpad3,
        sf::Keyboard::Up, sf::Keyboard::RControl),
    running(true),
    winner(0),
    hit1ThisFrame(false),
    hit2ThisFrame(false)
{
    if (!music.openFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Music/retro-arcade-game-music-297305.mp3")) {
        std::cout << "ERROR: Failed to load background music" << std::endl;
    }
    music.setLoop(true);
    music.setVolume(50);
    music.play();
}

// ADD DESTRUCTOR TO STOP MUSIC BEFORE DESTRUCTION
Game::~Game() {
    music.stop();  // CRITICAL: Stop music before object is destroyed
}

void Game::reset() {
    player1 = Fighter(
        200.f, window.getSize().y - 128.f * 2.0f - 12,
        sf::Keyboard::A, sf::Keyboard::D,
        sf::Keyboard::F, sf::Keyboard::G, sf::Keyboard::H,
        sf::Keyboard::W, sf::Keyboard::Q);

    player2 = Fighter(
        700.f, window.getSize().y - 128.f * 2.0f - 12,
        sf::Keyboard::Left, sf::Keyboard::Right,
        sf::Keyboard::Numpad1, sf::Keyboard::Numpad2, sf::Keyboard::Numpad3,
        sf::Keyboard::Up, sf::Keyboard::RControl);

    running = true;
    winner = 0;
    hit1ThisFrame = false;
    hit2ThisFrame = false;
}

void Game::update(float dt) {
    player1.update(dt, window);
    player2.update(dt, window);

    if (player1.isAttacking()) {
        bool hitboxOverlap = player1.getAttackHitbox().intersects(player2.getHitbox());
        bool closeRange = player1.getHitbox().intersects(player2.getHitbox());

        if ((hitboxOverlap || closeRange) && !player2.isShielding() && !hit2ThisFrame) {
            player2.hurt(10);
            hit2ThisFrame = true;
        }
    }
    else {
        hit2ThisFrame = false;
    }

    if (player2.isAttacking()) {
        bool hitboxOverlap = player2.getAttackHitbox().intersects(player1.getHitbox());
        bool closeRange = player2.getHitbox().intersects(player1.getHitbox());

        if ((hitboxOverlap || closeRange) && !player1.isShielding() && !hit1ThisFrame) {
            player1.hurt(10);
            hit1ThisFrame = true;
        }
    }
    else {
        hit1ThisFrame = false;
    }

    if (player1.getHP() <= 0 || player2.getHP() <= 0)
        running = false;

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        running = false;
}

void Game::render() {
    arena.draw(window);
    player1.draw(window);
    player2.draw(window);
    player1.drawHealthBar(window, true);
    player2.drawHealthBar(window, false);
}

bool Game::isRunning() const {
    return running;
}

int Game::getWinner() const {
    if (player1.getHP() <= 0 && player2.getHP() <= 0) return 0;
    if (player1.getHP() <= 0) return 2;
    if (player2.getHP() <= 0) return 1;
    return 0;
}

void Game::setMapBackground(int mapIndex) {
    std::string mapPath;

    switch (mapIndex) {
    case 0:
        mapPath = "D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/Background/arena.jpg";
        break;
    case 1:
        mapPath = "D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/Background/desert.jpg";
        break;
    case 2:
        mapPath = "D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/Background/forest.jpg";
        break;
    default:
        mapPath = "D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/Background/arena.jpg";
    }

    arena.setBackground(mapPath);
}
