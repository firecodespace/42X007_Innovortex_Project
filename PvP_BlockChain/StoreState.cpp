#include "StoreState.h"

#include "ResourcePaths.h"
#include "Wallets.h"
#include "http_client.h"
#include "json_util.h"

#include <sstream>

namespace {

bool healthLooksOk(const std::string& body) {
    return body.find("\"status\"") != std::string::npos && body.find("ok") != std::string::npos;
}

}  // namespace

StoreState::StoreState(sf::RenderWindow& win)
    : window(win),
      arena(findResourcePath("Resources/Images/Background/arena.jpg"), win.getSize()),
      bridge_(loadBridgeSettings()) {
    font.loadFromFile(findResourcePath("Resources/Images/fonts/ARCADECLASSIC.TTF"));
    initMainMenu();
    initWalletMenu();
    initWeaponMenu();
    initUpgradeMenu();
}

void StoreState::setBridgeStatus(const std::string& line) {
    std::lock_guard<std::mutex> lock(bridgeMutex_);
    bridgeStatusLine_ = line;
}

void StoreState::setPendingWalletLabel(const std::string& line) {
    std::lock_guard<std::mutex> lock(bridgeMutex_);
    pendingWalletLabel_ = line;
}

void StoreState::applyPendingUiFromBridge() {
    std::lock_guard<std::mutex> lock(bridgeMutex_);
    if (!pendingWalletLabel_.empty()) {
        walletTexts[1].setString(pendingWalletLabel_);
        pendingWalletLabel_.clear();
    }
}

bool StoreState::tryBeginBridgeRequest() {
    std::lock_guard<std::mutex> lock(bridgeMutex_);
    if (bridgeBusy_) {
        return false;
    }
    bridgeBusy_ = true;
    return true;
}

void StoreState::endBridgeRequest() {
    std::lock_guard<std::mutex> lock(bridgeMutex_);
    bridgeBusy_ = false;
}

bool StoreState::isPlausibleWallet(const std::string& w) {
    if (w.size() < 4) {
        return false;
    }
    if (w.rfind("0x", 0) != 0 && w.rfind("0X", 0) != 0) {
        return true;
    }
    return w.size() >= 42;
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
    const std::string labels[] = {"WALLET / BRIDGE", "BUY WEAPON (CHAIN)", "OFFLINE UPGRADES", "BACK TO MENU"};
    const float x = window.getSize().x / 2.f - 160;
    float y = 180.f;
    const float spacing = 55.f;
    for (int i = 0; i < 4; ++i) {
        sf::Text t;
        initText(t, labels[i], x, y + i * spacing);
        mainButtons.push_back(t);
    }
}

void StoreState::initWalletMenu() {
    walletTexts.clear();
    const std::string texts[] = {
        "BRIDGE",
        "STATUS: NOT CONNECTED",
        "PING BRIDGE (/health)",
        "MINT 50 AGLD TO PLAYER1 WALLET",
        "CLEAR BRIDGE STATUS",
        "PRESS ESC TO RETURN"};
    const float x = window.getSize().x / 2.f - 220;
    float y = 120.f;
    const float spacing = 48.f;
    for (int i = 0; i < 6; ++i) {
        sf::Text t;
        initText(t, texts[i], x, y + i * spacing);
        if (i == 1) {
            t.setCharacterSize(22);
        }
        walletTexts.push_back(t);
    }
}

void StoreState::initWeaponMenu() {
    weaponTexts.clear();
    const std::string texts[] = {
        "WEAPONS (ERC-1155 via bridge)",
        "Uses Player1 wallet from main menu",
        "MINT IRON SWORD (id 1)",
        "MINT AEGIS SHIELD (id 2)",
        "PRESS ESC TO RETURN"};
    const float x = window.getSize().x / 2.f - 220;
    float y = 130.f;
    const float spacing = 52.f;
    for (int i = 0; i < 5; ++i) {
        sf::Text t;
        initText(t, texts[i], x, y + i * spacing);
        if (i == 1) {
            t.setCharacterSize(20);
            t.setFillColor(sf::Color(200, 200, 200));
        }
        weaponTexts.push_back(t);
    }
}

void StoreState::initUpgradeMenu() {
    upgradeTexts.clear();
    const std::string texts[] = {
        "OFFLINE DEMO ONLY",
        "GOLD (LOCAL) 250",
        "UPGRADE ARMOR 50G",
        "UPGRADE WEAPON 50G",
        "UPGRADE HEALTH 40G",
        "PRESS ESC TO RETURN"};
    const float x = window.getSize().x / 2.f - 150;
    float y = 130.f;
    const float spacing = 50.f;
    for (int i = 0; i < 6; ++i) {
        sf::Text t;
        initText(t, texts[i], x, y + i * spacing);
        if (i == 0) {
            t.setCharacterSize(22);
            t.setFillColor(sf::Color(255, 200, 120));
        }
        upgradeTexts.push_back(t);
    }
}

void StoreState::drawTexts(const std::vector<sf::Text>& texts) {
    for (const auto& t : texts) {
        window.draw(t);
    }
}

StateID StoreState::update(float /*dt*/) {
    applyPendingUiFromBridge();

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            return StateID::Exit;
        }

        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
            if (currentSubMenu == SubMenu::Main) {
                return StateID::MainMenu;
            }
            currentSubMenu = SubMenu::Main;
        }

        if (event.type != sf::Event::MouseButtonPressed) {
            continue;
        }

        const auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (currentSubMenu == SubMenu::Main) {
            for (size_t i = 0; i < mainButtons.size(); ++i) {
                if (!mainButtons[i].getGlobalBounds().contains(pos)) {
                    continue;
                }
                switch (i) {
                case 0:
                    currentSubMenu = SubMenu::Wallet;
                    break;
                case 1:
                    currentSubMenu = SubMenu::BuyWeapon;
                    break;
                case 2:
                    currentSubMenu = SubMenu::Upgrade;
                    break;
                case 3:
                    return StateID::MainMenu;
                default:
                    break;
                }
            }
            continue;
        }

        if (currentSubMenu == SubMenu::Wallet) {
            if (!walletTexts[2].getGlobalBounds().contains(pos) && !walletTexts[3].getGlobalBounds().contains(pos) &&
                !walletTexts[4].getGlobalBounds().contains(pos)) {
                continue;
            }

            if (walletTexts[4].getGlobalBounds().contains(pos)) {
                walletConnected_.store(false);
                walletTexts[1].setString("STATUS: NOT CONNECTED");
                setBridgeStatus("");
                continue;
            }

            if (walletTexts[2].getGlobalBounds().contains(pos)) {
                if (!tryBeginBridgeRequest()) {
                    setBridgeStatus("Bridge request already running.");
                    continue;
                }
                const std::string url = joinUrl(bridge_.baseUrl, "/health");
                getUrlAsync(url, [this](bool success, std::string resp) {
                    endBridgeRequest();
                    if (!success) {
                        walletConnected_.store(false);
                        setPendingWalletLabel("STATUS: ERROR");
                        setBridgeStatus("Health check failed: " + resp);
                        return;
                    }
                    walletConnected_.store(healthLooksOk(resp));
                    setPendingWalletLabel(walletConnected_ ? "STATUS: BRIDGE REACHABLE" : "STATUS: UNEXPECTED RESPONSE");
                    setBridgeStatus(walletConnected_ ? "Bridge OK." : resp.substr(0, 200));
                });
                continue;
            }

            if (walletTexts[3].getGlobalBounds().contains(pos)) {
                const std::string to = g_player1_wallet.empty() ? "" : g_player1_wallet;
                if (!isPlausibleWallet(to)) {
                    setBridgeStatus("Set Player1 wallet on the main menu first.");
                    continue;
                }
                if (!tryBeginBridgeRequest()) {
                    setBridgeStatus("Bridge request already running.");
                    continue;
                }
                std::ostringstream body;
                body << "{\"to\":\"" << jsonEscape(to) << "\",\"amount\":\"50\"}";
                const std::string url = joinUrl(bridge_.baseUrl, "/currency/mint");
                postJsonAsync(url, body.str(), [this](bool success, std::string resp) {
                    endBridgeRequest();
                    if (!success || !jsonStatusSuccess(resp)) {
                        setBridgeStatus("Mint AGLD failed: " + resp);
                        return;
                    }
                    const auto tx = extractJsonString(resp, "txHash");
                    setBridgeStatus(tx.has_value() ? ("AGLD mint tx: " + *tx) : "AGLD mint OK.");
                });
            }
            continue;
        }

        if (currentSubMenu == SubMenu::BuyWeapon) {
            if (!weaponTexts[2].getGlobalBounds().contains(pos) && !weaponTexts[3].getGlobalBounds().contains(pos)) {
                continue;
            }
            const std::string to = g_player1_wallet.empty() ? "" : g_player1_wallet;
            if (!isPlausibleWallet(to)) {
                setBridgeStatus("Set Player1 wallet on the main menu first.");
                continue;
            }
            const int weaponId = weaponTexts[2].getGlobalBounds().contains(pos) ? 1 : 2;
            if (!tryBeginBridgeRequest()) {
                setBridgeStatus("Bridge request already running.");
                continue;
            }
            std::ostringstream body;
            body << "{\"to\":\"" << jsonEscape(to) << "\",\"weaponId\":" << weaponId << ",\"amount\":1}";
            const std::string url = joinUrl(bridge_.baseUrl, "/weapons/mint");
            postJsonAsync(url, body.str(), [this, weaponId](bool success, std::string resp) {
                endBridgeRequest();
                if (!success || !jsonStatusSuccess(resp)) {
                    setBridgeStatus("Weapon mint failed: " + resp);
                    return;
                }
                const auto tx = extractJsonString(resp, "txHash");
                setBridgeStatus(tx.has_value() ? ("Weapon " + std::to_string(weaponId) + " tx: " + *tx)
                                                : "Weapon mint OK.");
            });
            continue;
        }

        if (currentSubMenu == SubMenu::Upgrade) {
            if (upgradeTexts[2].getGlobalBounds().contains(pos)) {
                if (localGold_ >= 50) {
                    localGold_ -= 50;
                    upgradeTexts[1].setString("GOLD (LOCAL) " + std::to_string(localGold_));
                }
            } else if (upgradeTexts[3].getGlobalBounds().contains(pos)) {
                if (localGold_ >= 50) {
                    localGold_ -= 50;
                    upgradeTexts[1].setString("GOLD (LOCAL) " + std::to_string(localGold_));
                }
            } else if (upgradeTexts[4].getGlobalBounds().contains(pos)) {
                if (localGold_ >= 40) {
                    localGold_ -= 40;
                    upgradeTexts[1].setString("GOLD (LOCAL) " + std::to_string(localGold_));
                }
            }
        }
    }

    return StateID::Store;
}

void StoreState::render() {
    window.clear();
    arena.draw(window);
    sf::RectangleShape overlay(sf::Vector2f(static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)));
    overlay.setFillColor(sf::Color(0, 0, 0, 190));
    window.draw(overlay);

    switch (currentSubMenu) {
    case SubMenu::Main:
        drawTexts(mainButtons);
        break;
    case SubMenu::Wallet:
        drawTexts(walletTexts);
        break;
    case SubMenu::BuyWeapon:
        drawTexts(weaponTexts);
        break;
    case SubMenu::Upgrade:
        drawTexts(upgradeTexts);
        break;
    }

    std::string statusCopy;
    {
        std::lock_guard<std::mutex> lock(bridgeMutex_);
        statusCopy = bridgeStatusLine_;
    }
    if (!statusCopy.empty()) {
        sf::Text line(statusCopy, font, 18);
        line.setFillColor(sf::Color::Yellow);
        line.setPosition(24.f, static_cast<float>(window.getSize().y) - 80.f);
        window.draw(line);
    }

    window.display();
}
