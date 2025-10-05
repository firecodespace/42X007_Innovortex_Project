#pragma once
#include <SFML/Graphics.hpp>
#include <string>

class BackgroundVideo {
public:
    BackgroundVideo(const std::string& imagePath, sf::Vector2u windowSize);

    void draw(sf::RenderWindow& window);

private:
    sf::Texture texture;
    sf::Sprite sprite;
};
