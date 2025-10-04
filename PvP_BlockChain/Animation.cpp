#include "Animation.h"
Animation::Animation(sf::Texture& tex, int fw, int fh, int frames, bool loop)
    : texture(tex), frameWidth(fw), frameHeight(fh), frames(frames),
    loop(loop), currentFrame(0), frameTime(0.14f), elapsedTime(0), done(false) {
}
void Animation::update(float dt) {
    if (!loop && done) return;
    elapsedTime += dt;
    if (elapsedTime >= frameTime) {
        elapsedTime = 0;
        currentFrame++;
        if (currentFrame >= frames) {
            if (loop) currentFrame = 0;
            else { currentFrame = frames - 1; done = true; }
        }
    }
}
void Animation::applyToSprite(sf::Sprite& sprite) {
    sprite.setTexture(texture); sprite.setTextureRect(sf::IntRect(currentFrame * frameWidth, 0, frameWidth, frameHeight));
}
void Animation::reset() { currentFrame = 0; elapsedTime = 0; done = false; }
