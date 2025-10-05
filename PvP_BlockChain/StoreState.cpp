#include "StoreState.h"

StoreState::StoreState(sf::RenderWindow& win)
    : window(win),
    arena("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/Background/arena.jpg", win.getSize()) {
    font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");
    initMainMenu();
    initWalletMenu();
    initWeaponMenu();
    initUpgradeMenu();
}

void StoreState::initText(sf::Text& text, const std::string& str, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(28);
    text.setFillColor(sf::Color::White);
    text.setPosition(x, y);
}

void StoreState::initMainMenu() {
    mainButtons.clear();
    std::string labels[] = { "WALLET", "BUY WEAPON", "UPGRADES", "BACK TO MENU" };
    float x = window.getSize().x / 2.f - 100;
    float y = 180.f;
    float spacing = 55.f;
    for (int i = 0; i < 4; ++i) {
        sf::Text t;
        initText(t, labels[i], x, y + i * spacing);
        mainButtons.push_back(t);
    }
}

void StoreState::initWalletMenu() {
    walletTexts.clear();
    std::string texts[] = { "WALLET", "STATUS NOT CONNECTED", "CONNECT WALLET", "DISCONNECT WALLET", "PRESS ESC TO RETURN" };
    float x = window.getSize().x / 2.f - 150;
    float y = 150.f;
    float spacing = 55.f;
    for (int i = 0; i < 5; ++i) {
        sf::Text t;
        initText(t, texts[i], x, y + i * spacing);
        walletTexts.push_back(t);
    }
}

void StoreState::initWeaponMenu() {
    weaponTexts.clear();
    std::string texts[] = { "BUY WEAPONS", "GOLD 250", "BUY SWORD 80G", "BUY SHIELD 60G", "PRESS ESC TO RETURN" };
    float x = window.getSize().x / 2.f - 150;
    float y = 150.f;
    float spacing = 55.f;
    for (int i = 0; i < 5; ++i) {
        sf::Text t;
        initText(t, texts[i], x, y + i * spacing);
        weaponTexts.push_back(t);
    }
}

void StoreState::initUpgradeMenu() {
    upgradeTexts.clear();
    std::string texts[] = { "UPGRADES", "GOLD 250", "UPGRADE ARMOR 50G", "UPGRADE WEAPON 50G", "UPGRADE HEALTH 40G", "PRESS ESC TO RETURN" };
    float x = window.getSize().x / 2.f - 150;
    float y = 130.f;
    float spacing = 50.f;
    for (int i = 0; i < 6; ++i) {
        sf::Text t;
        initText(t, texts[i], x, y + i * spacing);
        upgradeTexts.push_back(t);
    }
}

void StoreState::drawTexts(const std::vector<sf::Text>& texts) {
    for (const auto& t : texts) window.draw(t);
}

StateID StoreState::update(float dt) {  // CHANGED: Added float dt parameter
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            return StateID::Exit;

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            if (currentSubMenu == SubMenu::Main)
                return StateID::MainMenu;
            else
                currentSubMenu = SubMenu::Main;
        }

        if (event.type == sf::Event::MouseButtonPressed) {
            auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

            if (currentSubMenu == SubMenu::Main) {
                for (size_t i = 0; i < mainButtons.size(); ++i) {
                    if (mainButtons[i].getGlobalBounds().contains(pos)) {
                        switch (i) {
                        case 0: currentSubMenu = SubMenu::Wallet; break;
                        case 1: currentSubMenu = SubMenu::BuyWeapon; break;
                        case 2: currentSubMenu = SubMenu::Upgrade; break;
                        case 3: return StateID::MainMenu;
                        }
                    }
                }
            }
            else if (currentSubMenu == SubMenu::Wallet) {
                if (walletTexts[2].getGlobalBounds().contains(pos)) {
                    walletConnected = true;
                    walletTexts[1].setString("STATUS CONNECTED");
                }
                if (walletTexts[3].getGlobalBounds().contains(pos)) {
                    walletConnected = false;
                    walletTexts[1].setString("STATUS NOT CONNECTED");
                }
            }
            else if (currentSubMenu == SubMenu::BuyWeapon) {
                if (weaponTexts[2].getGlobalBounds().contains(pos)) {
                    if (gold >= 80) {
                        gold -= 80;
                        weaponTexts[1].setString("GOLD " + std::to_string(gold));
                    }
                }
                if (weaponTexts[3].getGlobalBounds().contains(pos)) {
                    if (gold >= 60) {
                        gold -= 60;
                        weaponTexts[1].setString("GOLD " + std::to_string(gold));
                    }
                }
            }
            else if (currentSubMenu == SubMenu::Upgrade) {
                if (upgradeTexts[2].getGlobalBounds().contains(pos)) {
                    if (gold >= 50) {
                        gold -= 50;
                        upgradeTexts[1].setString("GOLD " + std::to_string(gold));
                    }
                }
                if (upgradeTexts[3].getGlobalBounds().contains(pos)) {
                    if (gold >= 50) {
                        gold -= 50;
                        upgradeTexts[1].setString("GOLD " + std::to_string(gold));
                    }
                }
                if (upgradeTexts[4].getGlobalBounds().contains(pos)) {
                    if (gold >= 40) {
                        gold -= 40;
                        upgradeTexts[1].setString("GOLD " + std::to_string(gold));
                    }
                }
            }
        }
    }
    return StateID::Store;
}

void StoreState::render() {
    window.clear();
    arena.draw(window);
    sf::RectangleShape overlay(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 190));
    window.draw(overlay);

    switch (currentSubMenu) {
    case SubMenu::Main: drawTexts(mainButtons); break;
    case SubMenu::Wallet: drawTexts(walletTexts); break;
    case SubMenu::BuyWeapon: drawTexts(weaponTexts); break;
    case SubMenu::Upgrade: drawTexts(upgradeTexts); break;
    }

    window.display();
}
