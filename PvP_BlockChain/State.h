#pragma once
#include <SFML/Graphics.hpp>

enum class StateID {
    MainMenu,
    GamePlay,
    PauseGame,
    Exit,
    Settings,
    Store
};

// Base class for all game states
class State {
public:
    virtual ~State() = default;
    virtual StateID update(float dt) = 0;
    virtual void render() = 0;
};
