#pragma once
#include <SFML/Graphics.hpp>

class BackgroundVideo {
public:
    BackgroundVideo(const std::string& spritesheetPath, int rows, int cols, float fps, const sf::Vector2u& windowSize);
    void update(float dt);
    void draw(sf::RenderWindow& window);
private:
    sf::Texture spritesheet;
    sf::Sprite sprite;
    int rows, cols, totalFrames;
    int currentFrame = 0;
    float frameTime;
    float elapsedTime = 0.f;
    int frameWidth, frameHeight;
};
