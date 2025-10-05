#include <SFML/Graphics.hpp>
#include "State.h"
#include "MainMenuState.h"
#include "GamePlayState.h"
#include "PauseGameState.h"
#include "StoreState.h"
#include "MapSelectionState.h"

int main() {
    sf::RenderWindow window(sf::VideoMode(960, 540), "Pixel PvP Arena");
    StateID currentState = StateID::MainMenu;
    StateID previousState = StateID::MainMenu;

    // Create state pointers
    State* mainMenu = nullptr;
    State* gamePlay = nullptr;
    State* pauseMenu = nullptr;
    State* storeMenu = nullptr;
    State* mapSelection = nullptr;

    sf::Clock clock;

    while (window.isOpen() && currentState != StateID::Exit) {
        float dt = clock.restart().asSeconds();

        // Recreate state ONLY when transitioning
        if (currentState != previousState) {
            // Clean up old state
            if (previousState == StateID::MainMenu) delete mainMenu;
            if (previousState == StateID::GamePlay) delete gamePlay;
            if (previousState == StateID::PauseGame) delete pauseMenu;
            if (previousState == StateID::Store) delete storeMenu;
            if (previousState == StateID::MapSelection) delete mapSelection;  // ADD THIS

            // Create new state
            if (currentState == StateID::MainMenu) mainMenu = new MainMenuState(window);
            if (currentState == StateID::GamePlay) gamePlay = new GamePlayState(window);
            if (currentState == StateID::PauseGame) pauseMenu = new PauseGameState(window);
            if (currentState == StateID::Store) storeMenu = new StoreState(window);
            if (currentState == StateID::MapSelection) mapSelection = new MapSelectionState(window);  // ADD THIS

            previousState = currentState;
        }

        // Run current state
        switch (currentState) {
        case StateID::MainMenu:
            if (!mainMenu) mainMenu = new MainMenuState(window);
            currentState = mainMenu->update(dt);
            mainMenu->render();
            break;
        case StateID::MapSelection:  // ADD THIS CASE
            if (!mapSelection) mapSelection = new MapSelectionState(window);
            currentState = mapSelection->update(dt);
            mapSelection->render();
            break;
        case StateID::GamePlay:
            if (!gamePlay) gamePlay = new GamePlayState(window);
            currentState = gamePlay->update(dt);
            gamePlay->render();
            break;
        case StateID::PauseGame:
            if (!pauseMenu) pauseMenu = new PauseGameState(window);
            currentState = pauseMenu->update(dt);
            pauseMenu->render();
            break;
        case StateID::Store:
            if (!storeMenu) storeMenu = new StoreState(window);
            currentState = storeMenu->update(dt);
            storeMenu->render();
            break;
        case StateID::Exit:
            window.close();
            break;
        default: break;
        }
    }

    // Cleanup
    delete mainMenu;
    delete gamePlay;
    delete pauseMenu;
    delete storeMenu;
    delete mapSelection;  // ADD THIS

    return 0;
}
