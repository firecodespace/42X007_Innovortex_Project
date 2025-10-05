#include "Fighter.h"
#include <iostream>

#define GRAVITY 900.0f
#define JUMP_STRENGTH 420.0f

Fighter::Fighter(float x, float y, sf::Keyboard::Key left, sf::Keyboard::Key right,
    sf::Keyboard::Key a1, sf::Keyboard::Key a2, sf::Keyboard::Key a3,
    sf::Keyboard::Key jKey, sf::Keyboard::Key sKey)
    : leftKey(left), rightKey(right), attack1Key(a1), attack2Key(a2), attack3Key(a3),
    jumpKey(jKey), shieldKey(sKey), velocityY(0), onGround(true), hp(100)
{
    loadAnimations();
    sprite.setPosition(x, y);
    sprite.setScale((left == sf::Keyboard::Left) ? -2.0f : 2.0f, 2.0f);
    sprite.setOrigin(0, 0);
    currentName = previousName = "IDLE";
    currentAnimation = animations["IDLE"];
}

Fighter::~Fighter() {
    for (auto& pair : animations) delete pair.second;
}

void Fighter::loadAnimations() {
    static sf::Texture textures[12];
    const char* files[] = { "Idle", "Walk", "Run", "Jump", "Attack 1", "Attack 2",
                           "Attack 3", "Hurt", "Dead", "Defend", "Protect", "Run+Attack" };
    for (int i = 0; i < 12; ++i) {
        std::string path = "Resources/Images/sprites/" + std::string(files[i]) + ".png";
        if (!textures[i].loadFromFile(path))
            std::cout << "ERROR: Failed to load " << files[i] << ".png" << std::endl;
    }

    animations["IDLE"] = new Animation(textures[0], 128, 128, 4, true);
    animations["WALK"] = new Animation(textures[1], 128, 128, 8, true);
    animations["RUN"] = new Animation(textures[2], 128, 128, 7, true);
    animations["JUMP"] = new Animation(textures[3], 128, 128, 6, true);
    animations["ATTACK1"] = new Animation(textures[4], 128, 128, 6, false);
    animations["ATTACK2"] = new Animation(textures[5], 128, 128, 6, false);
    animations["ATTACK3"] = new Animation(textures[6], 128, 128, 4, false);
    animations["HURT"] = new Animation(textures[7], 128, 128, 4, false);
    animations["DEAD"] = new Animation(textures[8], 128, 128, 6, false);
    animations["DEFEND"] = new Animation(textures[9], 128, 128, 2, true);
    animations["PROTECT"] = new Animation(textures[10], 128, 128, 2, true);
    animations["RUN_ATTACK"] = new Animation(textures[11], 128, 128, 6, false);
}

void Fighter::update(float dt, const sf::RenderWindow& window) {
    previousName = currentName;
    std::string name = "IDLE";
    sf::Vector2f move(0, 0);

    if (sf::Keyboard::isKeyPressed(leftKey)) { move.x -= 180 * dt; name = "WALK"; }
    if (sf::Keyboard::isKeyPressed(rightKey)) { move.x += 180 * dt; name = "WALK"; }
    if (sf::Keyboard::isKeyPressed(shieldKey)) name = "DEFEND";
    else {
        if (sf::Keyboard::isKeyPressed(attack1Key)) name = "ATTACK1";
        else if (sf::Keyboard::isKeyPressed(attack2Key)) name = "ATTACK2";
        else if (sf::Keyboard::isKeyPressed(attack3Key)) name = "ATTACK3";
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

    sf::FloatRect bounds = sprite.getGlobalBounds();
    sf::Vector2f pos = sprite.getPosition();
    float winWidth = static_cast<float>(window.getSize().x);

    pos.x = (sprite.getScale().x > 0) ?
        std::max(0.f, std::min(pos.x, winWidth - bounds.width)) :
        std::max(bounds.width, std::min(pos.x, winWidth));

    float groundY = window.getSize().y - bounds.height - 12;
    if (pos.y >= groundY) {
        pos.y = groundY;
        onGround = true;
        velocityY = 0.f;
    }
    else {
        onGround = false;
    }
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
    float x = left ? 20.f : window.getSize().x - 170.f;
    float y = left ? window.getSize().y - 56.f : 30.f;

    sf::RectangleShape bg({ 150, 16 });
    bg.setPosition(x, y);
    bg.setFillColor(sf::Color(83, 62, 46));

    sf::RectangleShape bar({ 150 * std::max(0.f, std::min(1.f, hp / 100.f)), 16 });
    bar.setPosition(x, y);
    bar.setFillColor(sf::Color(211, 58, 50));

    sf::RectangleShape outline({ 150, 16 });
    outline.setPosition(x, y);
    outline.setFillColor(sf::Color::Transparent);
    outline.setOutlineThickness(3);
    outline.setOutlineColor(sf::Color::Black);

    static sf::Font font;
    static bool fontLoaded = false;
    if (!fontLoaded) {
        font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");
        fontLoaded = true;
    }

    sf::Text label(left ? "PLAYER 1" : "PLAYER 2", font, 18);
    label.setFillColor(sf::Color::Black);
    label.setPosition(x, y - 24);

    sf::Text value(std::to_string(hp), font, 15);
    value.setFillColor(sf::Color::Black);
    value.setPosition(x, y + 19);

    window.draw(bg);
    window.draw(bar);
    window.draw(outline);
    window.draw(label);
    window.draw(value);
}

int Fighter::getHP() const { return hp; }
void Fighter::hurt(int amt) { hp = std::max(0, hp - amt); }

sf::FloatRect Fighter::getHitbox() const {
    sf::FloatRect bounds = sprite.getGlobalBounds();
    float w = bounds.width * 0.40f;
    float h = bounds.height * 0.70f;
    return sf::FloatRect(bounds.left + (bounds.width - w) / 2,
        bounds.top + bounds.height - h, w, h);
}


sf::FloatRect Fighter::getAttackHitbox() const {
    sf::FloatRect body = getHitbox();
    float attackWidth = 50.f;   // Reduced from 80 - shorter reach
    float attackHeight = 80.f;  // Reduced from 100
    float x = (sprite.getScale().x > 0) ? body.left + body.width : body.left - attackWidth;
    return sf::FloatRect(x, body.top + (body.height - attackHeight) / 2, attackWidth, attackHeight);
}


bool Fighter::isAttacking() const {
    return currentName == "ATTACK1" || currentName == "ATTACK2" || currentName == "ATTACK3";
}

bool Fighter::isShielding() const { return currentName == "DEFEND"; }

bool Fighter::canDealDamage() const {
    return isAttacking();
}
