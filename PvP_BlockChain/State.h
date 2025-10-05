#pragma once
#include <SFML/Graphics.hpp>

enum class StateID {
    MainMenu,
    MapSelection,
    GamePlay,
    PauseGame,
    Exit,
    Store
};

class State {
public:
    virtual ~State() = default;
    virtual StateID update(float dt) = 0;
    virtual void render() = 0;
};
