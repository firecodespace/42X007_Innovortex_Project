+#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include "Game.h"
#include "BridgeClient.h"
#include "BridgeConfig.h"
#include <thread>
#include <mutex>
#include <atomic>
#include <string>

class GamePlayState {
public:
    GamePlayState(sf::RenderWindow& win);
    ~GamePlayState();

    StateID update();
    void render();

private:
    sf::RenderWindow& window;
    Game game;
    sf::Clock clock;
    bool gameOver = false;
    int winner = 0;
    sf::Font font;
    bool bridgeNotified = false; // ensure we call bridge once per match
    std::atomic<bool> mintInProgress{false};
    std::string mintStatus;
    std::mutex mintMutex;
    std::thread mintThread;
};
