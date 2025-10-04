#include "Arena.h"
#include "State.h"

Arena::Arena(const std::string& imgFile, const sf::Vector2u& winSize) {
    texture.loadFromFile(imgFile);
    sprite.setTexture(texture);
    sf::Vector2u texSize = texture.getSize();
    sprite.setScale(
        float(winSize.x) / texSize.x,
        float(winSize.y) / texSize.y
    );
}

void Arena::draw(sf::RenderWindow& window) {
    window.draw(sprite);
}
