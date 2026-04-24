#pragma once

#include <SFML/Graphics.hpp>
#include <atomic>
#include <mutex>
#include <string>

#include "BridgeConfig.h"
#include "State.h"
#include "Arena.h"

class StoreState : public State {
public:
    explicit StoreState(sf::RenderWindow& win);
    StateID update(float dt) override;
    void render() override;

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

    BridgeSettings bridge_;
    std::mutex bridgeMutex_;
    std::string bridgeStatusLine_;
    std::string pendingWalletLabel_;
    bool bridgeBusy_ = false;

    std::atomic<bool> walletConnected_{false};
    int localGold_ = 250;

    void initMainMenu();
    void initWalletMenu();
    void initWeaponMenu();
    void initUpgradeMenu();
    void drawTexts(const std::vector<sf::Text>& texts);
    void initText(sf::Text& text, const std::string& str, float x, float y);

    void setBridgeStatus(const std::string& line);
    void setPendingWalletLabel(const std::string& line);
    void applyPendingUiFromBridge();
    bool tryBeginBridgeRequest();
    void endBridgeRequest();
    static bool isPlausibleWallet(const std::string& w);
};
