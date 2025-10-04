#include "BackgroundVideo.h"
#include <iostream>

BackgroundVideo::BackgroundVideo(const std::string& spritesheetPath, int rows, int cols, float fps, const sf::Vector2u& windowSize)
    : rows(rows), cols(cols), frameTime(1.0f / fps) {

    if (!spritesheet.loadFromFile(spritesheetPath)) {
        std::cout << "Failed to load spritesheet: " << spritesheetPath << std::endl;
    }

    totalFrames = rows * cols;
    frameWidth = spritesheet.getSize().x / cols;
    frameHeight = spritesheet.getSize().y / rows;

    sprite.setTexture(spritesheet);
    sprite.setTextureRect(sf::IntRect(0, 0, frameWidth, frameHeight));

    // Scale to fit window
    sprite.setScale(
        float(windowSize.x) / frameWidth,
        float(windowSize.y) / frameHeight
    );
}

void BackgroundVideo::update(float dt) {
    elapsedTime += dt;
    if (elapsedTime >= frameTime) {
        elapsedTime = 0;
        currentFrame = (currentFrame + 1) % totalFrames;

        int col = currentFrame % cols;
        int row = currentFrame / cols;

        sprite.setTextureRect(sf::IntRect(col * frameWidth, row * frameHeight, frameWidth, frameHeight));
    }
}

void BackgroundVideo::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
