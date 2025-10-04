#include <SFML/Graphics.hpp>
#include "State.h"
#include "MainMenuState.h"
#include "GamePlayState.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(960, 540), "Pixel PvP Arena");
    StateID state = StateID::MainMenu;
    MainMenuState mainMenu(window);
    GamePlayState gamePlay(window);

    while (window.isOpen() && state != StateID::Exit) {
        switch (state) {
        case StateID::MainMenu:
            state = mainMenu.update();
            mainMenu.render();
            break;
        case StateID::GamePlay:
            state = gamePlay.update();
            gamePlay.render();
            break;
        case StateID::Settings: {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                    state = StateID::MainMenu;
            }
            window.clear(sf::Color(40, 100, 200));
            // Draw your settings UI here!
            window.display();
            break;
        }
        case StateID::Store: {
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed)
                    window.close();
                if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                    state = StateID::MainMenu;
            }
            window.clear(sf::Color(120, 40, 100));
            // Draw your store UI here!
            window.display();
            break;
        }
        case StateID::Exit:
            window.close();
            break;
        }
    }
    return 0;
}
