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
    StateID previousState = StateID::Exit;

    // Use separate pointers for each state type
    MainMenuState* mainMenu = nullptr;
    MapSelectionState* mapSelection = nullptr;
    GamePlayState* gamePlay = nullptr;
    PauseGameState* pauseMenu = nullptr;
    StoreState* storeMenu = nullptr;

    State* currentStatePtr = nullptr;

    sf::Clock clock;

    while (window.isOpen() && currentState != StateID::Exit) {
        float dt = clock.restart().asSeconds();

        // Only recreate state when transitioning
        if (currentState != previousState) {
            // Create states ONCE and reuse them (except GamePlay which is recreated)
            switch (currentState) {
            case StateID::MainMenu:
                if (!mainMenu) mainMenu = new MainMenuState(window);
                currentStatePtr = mainMenu;
                break;
            case StateID::MapSelection:
                if (!mapSelection) mapSelection = new MapSelectionState(window);
                currentStatePtr = mapSelection;
                break;
            case StateID::GamePlay:
                // CRITICAL FIX: Delete OLD gamePlay ONLY if it exists and we're coming FROM gameplay
                if (gamePlay && previousState == StateID::GamePlay) {
                    delete gamePlay;
                    gamePlay = nullptr;
                }
                // Now create fresh GamePlayState
                gamePlay = new GamePlayState(window);
                currentStatePtr = gamePlay;
                break;
            case StateID::PauseGame:
                if (!pauseMenu) pauseMenu = new PauseGameState(window);
                currentStatePtr = pauseMenu;
                break;
            case StateID::Store:
                if (!storeMenu) storeMenu = new StoreState(window);
                currentStatePtr = storeMenu;
                break;
            default:
                break;
            }

            previousState = currentState;
        }

        // Run current state
        if (currentStatePtr) {
            currentState = currentStatePtr->update(dt);
            currentStatePtr->render();
        }

        if (currentState == StateID::Exit) {
            window.close();
        }
    }

    // Cleanup all states
    if (mainMenu) delete mainMenu;
    if (mapSelection) delete mapSelection;
    if (gamePlay) delete gamePlay;
    if (pauseMenu) delete pauseMenu;
    if (storeMenu) delete storeMenu;

    return 0;
}
