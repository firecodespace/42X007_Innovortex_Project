#pragma once
#include <SFML/Graphics.hpp>
#include <map>
#include <string>
#include "Animation.h"

class Fighter {
public:
    Fighter(float x, float y,
        sf::Keyboard::Key left, sf::Keyboard::Key right,
        sf::Keyboard::Key a1, sf::Keyboard::Key a2, sf::Keyboard::Key a3,
        sf::Keyboard::Key jumpKey, sf::Keyboard::Key shieldKey);
    ~Fighter();

    void update(float dt, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window);
    void drawHealthBar(sf::RenderWindow& window, bool left) const;

    int getHP() const;
    void hurt(int amt);
    sf::FloatRect getHitbox() const;
    sf::FloatRect getAttackHitbox() const;
    bool isAttacking() const;
    bool isShielding() const;
    bool canDealDamage() const;  // NEW: Check if attack can deal damage
    void drawDebugHitbox(sf::RenderWindow& window) const;

private:
    void loadAnimations();
    sf::Sprite sprite;
    std::map<std::string, Animation*> animations;
    Animation* currentAnimation = nullptr;
    std::string currentName;
    std::string previousName;  // NEW: Track previous animation
    int hp = 100;
    sf::Keyboard::Key leftKey, rightKey, attack1Key, attack2Key, attack3Key, jumpKey, shieldKey;
    bool onGround = true;
    float velocityY = 0;
};
