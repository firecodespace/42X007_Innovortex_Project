#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class Arena {
public:
    Arena(const std::string& imgFile, const sf::Vector2u& winSize); // <-- add second argument
    void draw(sf::RenderWindow& window);

private:
    sf::Texture texture;
    sf::Sprite sprite;
};
