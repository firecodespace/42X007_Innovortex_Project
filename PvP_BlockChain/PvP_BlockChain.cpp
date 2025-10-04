#include <SFML/Graphics.hpp>
#include "State.h"
#include "MainMenuState.h"
#include "GamePlayState.h"
#include "PauseGameState.h"
#include "StoreState.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(960, 540), "Pixel PvP Arena");
    StateID state = StateID::MainMenu;
    MainMenuState mainMenu(window);
    GamePlayState gamePlay(window);
    PauseGameState pauseMenu(window);
    StoreState storeMenu(window);
    sf::Clock clock;

    while (window.isOpen() && state != StateID::Exit) {
        float dt = clock.restart().asSeconds();

        switch (state) {
        case StateID::MainMenu:
            state = mainMenu.update();
            mainMenu.render();
            break;
        case StateID::GamePlay:
            state = gamePlay.update(dt);
            gamePlay.render();
            break;
        case StateID::PauseGame:
            state = pauseMenu.update();
            pauseMenu.render();
            break;
        case StateID::Store:
            state = storeMenu.update();
            storeMenu.render();
            break;
        case StateID::Exit:
            window.close();
            break;
        default: break;
        }
    }
    return 0;
}
