#include "Arena.h"
#include <iostream>

Arena::Arena(const std::string& imagePath, const sf::Vector2u& windowSize)
    : windowSize(windowSize) {
    if (!texture.loadFromFile(imagePath)) {
        std::cout << "ERROR: Failed to load arena background: " << imagePath << std::endl;
    }
    else {
        sprite.setTexture(texture);
        sf::Vector2u textureSize = texture.getSize();

        // Calculate scale to FILL entire window
        float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
        float scaleY = static_cast<float>(windowSize.y) / textureSize.y;

        // Use the LARGER scale to ensure full coverage
        float scale = std::max(scaleX, scaleY);

        sprite.setScale(scale, scale);
        sprite.setPosition(0, 0);
    }
}

void Arena::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Arena::setBackground(const std::string& imagePath) {
    if (!texture.loadFromFile(imagePath)) {
        std::cout << "ERROR: Failed to load arena background: " << imagePath << std::endl;
        return;
    }

    sprite.setTexture(texture);
    sf::Vector2u textureSize = texture.getSize();

    // Calculate scale to FILL entire window (crop if needed)
    float scaleX = static_cast<float>(windowSize.x) / textureSize.x;
    float scaleY = static_cast<float>(windowSize.y) / textureSize.y;

    // Use LARGER scale to ensure full coverage (will crop edges)
    float scale = std::max(scaleX, scaleY);

    sprite.setScale(scale, scale);

    // Center the sprite
    float scaledWidth = textureSize.x * scale;
    float scaledHeight = textureSize.y * scale;
    float offsetX = (windowSize.x - scaledWidth) / 2.0f;
    float offsetY = (windowSize.y - scaledHeight) / 2.0f;

    sprite.setPosition(offsetX, offsetY);
}


