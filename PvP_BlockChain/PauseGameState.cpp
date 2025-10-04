#include "PauseGameState.h"

PauseGameState::PauseGameState(sf::RenderWindow& win)
    : window(win),
    arena("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/Background/arena.jpg", win.getSize()) {
    font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");
    initMainMenu();
    initWalletMenu();
    initPurchaseMenu();
    initUpgradeMenu();
}

void PauseGameState::initText(sf::Text& text, const std::string& str, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::White);
    text.setPosition(x, y);
}

void PauseGameState::initMainMenu() {
    mainButtons.clear();
    std::string labels[] = { "WALLET", "PURCHASE TOOLS", "UPGRADES", "EXIT" };
    float startX = window.getSize().x / 2.f - 100;
    float startY = 200.f;
    float spacing = 50.f;
    for (int i = 0; i < 4; i++) {
        sf::Text t;
        initText(t, labels[i], startX, startY + i * spacing);
        mainButtons.push_back(t);
    }
}

void PauseGameState::initWalletMenu() {
    walletMenuTexts.clear();
    std::string walletTexts[] = { "WALLET", "STATUS NOT CONNECTED", "CONNECT", "PRESS ESC TO RETURN" };
    float startX = window.getSize().x / 2.f - 150;
    float startY = 150.f;
    float spacing = 60.f;
    for (int i = 0; i < 4; i++) {
        sf::Text t;
        initText(t, walletTexts[i], startX, startY + i * spacing);
        walletMenuTexts.push_back(t);
    }
}

void PauseGameState::initPurchaseMenu() {
    purchaseMenuTexts.clear();
    std::string purchaseTexts[] = { "PURCHASE TOOLS", "GOLD 250", "BUY SWORD 80G", "BUY SHIELD 60G", "PRESS ESC TO RETURN" };
    float startX = window.getSize().x / 2.f - 150;
    float startY = 150.f;
    float spacing = 50.f;
    for (int i = 0; i < 5; i++) {
        sf::Text t;
        initText(t, purchaseTexts[i], startX, startY + i * spacing);
        purchaseMenuTexts.push_back(t);
    }
}

void PauseGameState::initUpgradeMenu() {
    upgradeMenuTexts.clear();
    std::string upgradeTexts[] = { "UPGRADES", "GOLD 250", "DAMAGE 1 50G", "HEALTH 10 50G", "SPEED 1 40G", "PRESS ESC TO RETURN" };
    float startX = window.getSize().x / 2.f - 150;
    float startY = 150.f;
    float spacing = 50.f;
    for (int i = 0; i < 6; ++i) {
        sf::Text t;
        initText(t, upgradeTexts[i], startX, startY + i * spacing);
        upgradeMenuTexts.push_back(t);
    }
}

void PauseGameState::drawTexts(const std::vector<sf::Text>& texts) {
    for (const auto& t : texts) window.draw(t);
}

StateID PauseGameState::update() {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            return StateID::Exit;
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Escape) {
                if (currentSubMenu == SubMenu::Main)
                    return StateID::GamePlay;
                else
                    currentSubMenu = SubMenu::Main;
            }
        }
        if (event.type == sf::Event::MouseButtonPressed) {
            auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            if (currentSubMenu == SubMenu::Main) {
                for (size_t i = 0; i < mainButtons.size(); ++i) {
                    if (mainButtons[i].getGlobalBounds().contains(pos)) {
                        switch (i) {
                        case 0: currentSubMenu = SubMenu::Wallet; break;
                        case 1: currentSubMenu = SubMenu::PurchaseTools; break;
                        case 2: currentSubMenu = SubMenu::Upgrades; break;
                        case 3: return StateID::MainMenu;
                        }
                    }
                }
            }
        }
    }
    return StateID::PauseGame;
}

void PauseGameState::render() {
    window.clear();
    arena.draw(window);
    sf::RectangleShape overlay(sf::Vector2f(window.getSize()));
    overlay.setFillColor(sf::Color(0, 0, 0, 190));
    window.draw(overlay);

    switch (currentSubMenu) {
    case SubMenu::Main:
        drawTexts(mainButtons);
        break;
    case SubMenu::Wallet:
        drawTexts(walletMenuTexts);
        break;
    case SubMenu::PurchaseTools:
        drawTexts(purchaseMenuTexts);
        break;
    case SubMenu::Upgrades:
        drawTexts(upgradeMenuTexts);
        break;
    }
    window.display();
}
