#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include "Arena.h"

class MapSelectionState : public State {
public:
    MapSelectionState(sf::RenderWindow& win);
    StateID update(float dt) override;
    void render() override;
    static int getSelectedMap() { return selectedMapIndex; }

private:
    sf::RenderWindow& window;
    sf::Font font;
    Arena arena;
    std::vector<sf::Text> mapButtons;
    static int selectedMapIndex;

    void initMapButtons();
    void initText(sf::Text& text, const std::string& str, float x, float y);
};
