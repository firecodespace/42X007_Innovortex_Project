#pragma once
#include <SFML/Graphics.hpp>
#include "State.h"
#include "Arena.h"

class StoreState : public State {  // ADD : public State
public:
    StoreState(sf::RenderWindow& win);
    StateID update(float dt) override;  // ADD float dt and override
    void render() override;  // ADD override
private:
    sf::RenderWindow& window;
    sf::Font font;
    Arena arena;

    enum class SubMenu { Main, Wallet, BuyWeapon, Upgrade };
    SubMenu currentSubMenu = SubMenu::Main;

    std::vector<sf::Text> mainButtons;
    std::vector<sf::Text> walletTexts;
    std::vector<sf::Text> weaponTexts;
    std::vector<sf::Text> upgradeTexts;

    bool walletConnected = false;
    int gold = 250;

    void initMainMenu();
    void initWalletMenu();
    void initWeaponMenu();
    void initUpgradeMenu();
    void drawTexts(const std::vector<sf::Text>& texts);
    void initText(sf::Text& text, const std::string& str, float x, float y);
};
