#include "Fighter.h"
#include <iostream>

#define GRAVITY 900.0f
#define JUMP_STRENGTH 420.0f

Fighter::Fighter(float x, float y, sf::Keyboard::Key left, sf::Keyboard::Key right,
    sf::Keyboard::Key a1, sf::Keyboard::Key a2, sf::Keyboard::Key a3,
    sf::Keyboard::Key jKey, sf::Keyboard::Key sKey)
    : leftKey(left), rightKey(right),
    attack1Key(a1), attack2Key(a2), attack3Key(a3),
    jumpKey(jKey), shieldKey(sKey)
{
    loadAnimations();
    sprite.setPosition(x, y);

    if (left == sf::Keyboard::Left)
        sprite.setScale(-2.0f, 2.0f);
    else
        sprite.setScale(2.0f, 2.0f);

    sprite.setOrigin(0, 0);
    currentName = "IDLE";
    currentAnimation = animations["IDLE"];
    velocityY = 0;
    onGround = true;
    hp = 100;
}

void Fighter::loadAnimations() {
    static sf::Texture idleTex, walkTex, runTex, jumpTex, atk1Tex, atk2Tex, atk3Tex, hurtTex, deadTex, defendTex, protectTex, runAtkTex;

    if (!idleTex.loadFromFile("Resources/Images/sprites/Idle.png"))
        std::cout << "ERROR: Failed to load Idle.png" << std::endl;
    if (!walkTex.loadFromFile("Resources/Images/sprites/Walk.png"))
        std::cout << "ERROR: Failed to load Walk.png" << std::endl;
    if (!runTex.loadFromFile("Resources/Images/sprites/Run.png"))
        std::cout << "ERROR: Failed to load Run.png" << std::endl;
    if (!jumpTex.loadFromFile("Resources/Images/sprites/Jump.png"))
        std::cout << "ERROR: Failed to load Jump.png" << std::endl;
    if (!atk1Tex.loadFromFile("Resources/Images/sprites/Attack 1.png"))
        std::cout << "ERROR: Failed to load Attack 1.png" << std::endl;
    if (!atk2Tex.loadFromFile("Resources/Images/sprites/Attack 2.png"))
        std::cout << "ERROR: Failed to load Attack 2.png" << std::endl;
    if (!atk3Tex.loadFromFile("Resources/Images/sprites/Attack 3.png"))
        std::cout << "ERROR: Failed to load Attack 3.png" << std::endl;
    if (!hurtTex.loadFromFile("Resources/Images/sprites/Hurt.png"))
        std::cout << "ERROR: Failed to load Hurt.png" << std::endl;
    if (!deadTex.loadFromFile("Resources/Images/sprites/Dead.png"))
        std::cout << "ERROR: Failed to load Dead.png" << std::endl;
    if (!defendTex.loadFromFile("Resources/Images/sprites/Defend.png"))
        std::cout << "ERROR: Failed to load Defend.png" << std::endl;
    if (!protectTex.loadFromFile("Resources/Images/sprites/Protect.png"))
        std::cout << "ERROR: Failed to load Protect.png" << std::endl;
    if (!runAtkTex.loadFromFile("Resources/Images/sprites/Run+Attack.png"))
        std::cout << "ERROR: Failed to load Run+Attack.png" << std::endl;

    animations["IDLE"] = new Animation(idleTex, 128, 128, 4, true);
    animations["WALK"] = new Animation(walkTex, 128, 128, 8, true);
    animations["RUN"] = new Animation(runTex, 128, 128, 7, true);
    animations["JUMP"] = new Animation(jumpTex, 128, 128, 6, true);
    animations["ATTACK1"] = new Animation(atk1Tex, 128, 128, 6, false);
    animations["ATTACK2"] = new Animation(atk2Tex, 128, 128, 6, false);
    animations["ATTACK3"] = new Animation(atk3Tex, 128, 128, 4, false);
    animations["HURT"] = new Animation(hurtTex, 128, 128, 4, false);
    animations["DEAD"] = new Animation(deadTex, 128, 128, 6, false);
    animations["DEFEND"] = new Animation(defendTex, 128, 128, 2, true);
    animations["PROTECT"] = new Animation(protectTex, 128, 128, 2, true);
    animations["RUN_ATTACK"] = new Animation(runAtkTex, 128, 128, 6, false);
}

void Fighter::update(float dt, const sf::RenderWindow& window) {
    std::string name = "IDLE";
    sf::Vector2f move(0, 0);

    if (sf::Keyboard::isKeyPressed(leftKey)) { move.x -= 180 * dt; name = "WALK"; }
    if (sf::Keyboard::isKeyPressed(rightKey)) { move.x += 180 * dt; name = "WALK"; }

    if (sf::Keyboard::isKeyPressed(shieldKey)) name = "DEFEND";
    else {
        if (sf::Keyboard::isKeyPressed(attack1Key)) name = "ATTACK1";
        if (sf::Keyboard::isKeyPressed(attack2Key)) name = "ATTACK2";
        if (sf::Keyboard::isKeyPressed(attack3Key)) name = "ATTACK3";
    }

    if (sf::Keyboard::isKeyPressed(jumpKey) && onGround) {
        velocityY = -JUMP_STRENGTH;
        onGround = false;
        name = "JUMP";
    }

    move.y += velocityY * dt;
    velocityY += GRAVITY * dt;
    currentAnimation->applyToSprite(sprite);
    sprite.move(move);

    sf::FloatRect globalBounds = sprite.getGlobalBounds();
    sf::Vector2f pos = sprite.getPosition();
    float windowWidth = static_cast<float>(window.getSize().x);

    if (sprite.getScale().x > 0)
        pos.x = std::max(0.f, std::min(pos.x, windowWidth - globalBounds.width));
    else
        pos.x = std::max(globalBounds.width, std::min(pos.x, windowWidth));
    float groundY = window.getSize().y - globalBounds.height - 12;
    if (pos.y >= groundY) {
        pos.y = groundY;
        onGround = true;
        velocityY = 0.f;
    }
    else onGround = false;
    sprite.setPosition(pos);

    if (name != currentName && (currentAnimation->isLooping() || currentAnimation->isComplete())) {
        currentName = name;
        currentAnimation = animations[name];
        currentAnimation->reset();
    }
    currentAnimation->update(dt);
}

void Fighter::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}

void Fighter::drawHealthBar(sf::RenderWindow& window, bool left) const {
    float barWidth = 150, barHeight = 16, x = left ? 20 : window.getSize().x - barWidth - 20,
        y = left ? window.getSize().y - barHeight - 40 : 30;
    sf::RectangleShape outline({ barWidth, barHeight });
    outline.setPosition(x, y);
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineThickness(3);
    outline.setOutlineColor(sf::Color::Black);
    sf::RectangleShape bg({ barWidth, barHeight });
    bg.setPosition(x, y);
    bg.setFillColor(sf::Color(83, 62, 46));
    float percent = std::max(0.f, std::min(1.f, float(hp) / 100.f));
    sf::RectangleShape bar({ barWidth * percent, barHeight });
    bar.setPosition(x, y);
    bar.setFillColor(sf::Color(211, 58, 50));
    sf::Font font;
    font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");
    sf::Text label(left ? "PLAYER 1" : "PLAYER 2", font, 18);
    label.setFillColor(sf::Color::Black);
    label.setPosition(x, y - 24);
    sf::Text value(std::to_string(hp), font, 15);
    value.setFillColor(sf::Color::Black);
    value.setPosition(x, y + barHeight + 3);
    window.draw(bg);
    window.draw(bar);
    window.draw(outline);
    window.draw(label);
    window.draw(value);
}

int Fighter::getHP() const { return hp; }
void Fighter::hurt(int amt) { hp -= amt; if (hp < 0) hp = 0; }
sf::FloatRect Fighter::getHitbox() const { return sprite.getGlobalBounds(); }
bool Fighter::isAttacking() const { return currentName == "ATTACK1" || currentName == "ATTACK2" || currentName == "ATTACK3"; }
bool Fighter::isShielding() const { return currentName == "DEFEND"; }
