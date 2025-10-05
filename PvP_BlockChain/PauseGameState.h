#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include "Arena.h"

class PauseGameState : public State {  // ADD : public State
public:
    PauseGameState(sf::RenderWindow& win);
    StateID update(float dt) override;  // ADD float dt and override
    void render() override;  // ADD override
private:
    sf::RenderWindow& window;
    sf::Font font;
    Arena arena;
    enum class SubMenu { Main, Wallet, PurchaseTools, Upgrades };
    SubMenu currentSubMenu = SubMenu::Main;
    std::vector<sf::Text> mainButtons, walletMenuTexts, purchaseMenuTexts, upgradeMenuTexts;
    void initMainMenu(), initWalletMenu(), initPurchaseMenu(), initUpgradeMenu();
    void drawTexts(const std::vector<sf::Text>& t);
    void initText(sf::Text& t, const std::string& str, float x, float y);
};
