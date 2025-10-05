#include "BackgroundVideo.h"
#include <iostream>

BackgroundVideo::BackgroundVideo(const std::string& imagePath, sf::Vector2u windowSize) {
    if (!texture.loadFromFile(imagePath)) {
        std::cout << "ERROR: Failed to load background: " << imagePath << std::endl;
    }

    sprite.setTexture(texture);

    // Scale to fit window
    float scaleX = float(windowSize.x) / texture.getSize().x;
    float scaleY = float(windowSize.y) / texture.getSize().y;
    sprite.setScale(scaleX, scaleY);
}

void BackgroundVideo::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
