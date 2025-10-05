#pragma once
#include <SFML/Graphics.hpp>

class Arena {
public:
    Arena(const std::string& imagePath, const sf::Vector2u& windowSize);
    void draw(sf::RenderWindow& window);
    void setBackground(const std::string& imagePath);

private:
    sf::Texture texture;
    sf::Sprite sprite;
    sf::Vector2u windowSize;
};
