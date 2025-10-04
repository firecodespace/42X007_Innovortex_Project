#pragma once
#include <SFML/Graphics.hpp>
class Animation {
public:
    Animation(sf::Texture& tex, int fw, int fh, int frames, bool loop = true);
    void update(float dt);
    void applyToSprite(sf::Sprite& sprite);
    void reset();
    bool isComplete() const { return done; }
    bool isLooping()   const { return loop; }
private:
    sf::Texture& texture;
    int frameWidth, frameHeight, frames, currentFrame;
    float frameTime, elapsedTime;
    bool done, loop;
};
