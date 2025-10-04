#include "BattleLog.h"
BattleLog::BattleLog() { font.loadFromFile("arial.ttf"); }
void BattleLog::addMessage(const std::string& msg) { if (messages.size() > 6) messages.pop_front(); messages.push_back(msg); }
void BattleLog::draw(sf::RenderWindow& window) {
    float y = 10;
    for (const auto& message : messages) {
        sf::Text text(message, font, 18);
        text.setPosition(10, y);
        text.setFillColor(sf::Color::White);
        window.draw(text);
        y += 24;
    }
}
