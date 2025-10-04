#pragma once
#include <SFML/Graphics.hpp>
#include <deque>
#include <string>
class BattleLog {
public:
    BattleLog();
    void addMessage(const std::string& msg);
    void draw(sf::RenderWindow& window);
private:
    std::deque<std::string> messages;
    sf::Font font;
};
