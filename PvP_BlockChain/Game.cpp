#include "Game.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <sstream>

#include "BridgeConfig.h"
#include "ResourcePaths.h"
#include "Wallets.h"
#include "http_client.h"
#include "json_util.h"

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

namespace {
constexpr float kGroundOffset = 12.0f;
constexpr int kDamagePerHit = 10;
}

Game::Game(sf::RenderWindow& win)
    : window(win),
      arena(findResourcePath("Resources/Images/Background/arena.jpg"), win.getSize()) {
    if (!music.openFromFile(findResourcePath("Resources/Music/retro-arcade-game-music-297305.mp3"))) {
        std::cout << "WARN: failed to load background music\n";
    } else {
        music.setLoop(true);
        music.setVolume(50.f);
        music.play();
    }

    if (!statusFont.loadFromFile(findResourcePath("Resources/Images/fonts/ARCADECLASSIC.TTF"))) {
        std::cout << "WARN: failed to load status font\n";
    }

    confirmButton.setSize({200.f, 50.f});
    confirmButton.setFillColor(sf::Color::Green);
    confirmButton.setPosition(300.f, 300.f);

    cancelButton.setSize({200.f, 50.f});
    cancelButton.setFillColor(sf::Color::Red);
    cancelButton.setPosition(520.f, 300.f);

    confirmText.setFont(statusFont);
    confirmText.setString("CONFIRM");
    confirmText.setCharacterSize(24);
    confirmText.setFillColor(sf::Color::Black);
    confirmText.setPosition(330.f, 310.f);

    cancelText.setFont(statusFont);
    cancelText.setString("CANCEL");
    cancelText.setCharacterSize(24);
    cancelText.setFillColor(sf::Color::Black);
    cancelText.setPosition(560.f, 310.f);

    copyTransferButton.setSize({180.f, 36.f});
    copyTransferButton.setFillColor(sf::Color(100, 100, 255));
    copyTransferButton.setPosition(100.f, 360.f);

    copyMintButton.setSize({180.f, 36.f});
    copyMintButton.setFillColor(sf::Color(100, 100, 255));
    copyMintButton.setPosition(320.f, 360.f);

    copyTokenButton.setSize({180.f, 36.f});
    copyTokenButton.setFillColor(sf::Color(100, 100, 255));
    copyTokenButton.setPosition(540.f, 360.f);

    copyTransferText.setFont(statusFont);
    copyTransferText.setString("COPY BATTLE TX");
    copyTransferText.setCharacterSize(16);
    copyTransferText.setFillColor(sf::Color::Black);
    copyTransferText.setPosition(108.f, 368.f);

    copyMintText.setFont(statusFont);
    copyMintText.setString("COPY MINT TX");
    copyMintText.setCharacterSize(16);
    copyMintText.setFillColor(sf::Color::Black);
    copyMintText.setPosition(338.f, 368.f);

    copyTokenText.setFont(statusFont);
    copyTokenText.setString("COPY TOKEN ID");
    copyTokenText.setCharacterSize(16);
    copyTokenText.setFillColor(sf::Color::Black);
    copyTokenText.setPosition(558.f, 368.f);

    reset();
}

Game::~Game() = default;

void Game::reset() {
    fighter1 = std::make_unique<Fighter>(
        200.f, window.getSize().y - 128.f * 2.0f - kGroundOffset,
        sf::Keyboard::A, sf::Keyboard::D,
        sf::Keyboard::F, sf::Keyboard::G, sf::Keyboard::H,
        sf::Keyboard::W, sf::Keyboard::Q);

    fighter2 = std::make_unique<Fighter>(
        700.f, window.getSize().y - 128.f * 2.0f - kGroundOffset,
        sf::Keyboard::Left, sf::Keyboard::Right,
        sf::Keyboard::Numpad1, sf::Keyboard::Numpad2, sf::Keyboard::Numpad3,
        sf::Keyboard::Up, sf::Keyboard::RControl);

    running = true;
    hit1ThisFrame = false;
    hit2ThisFrame = false;
    mintTriggered = false;
    awaitingConfirmation = false;
    mintSucceeded = false;
    statusTextString.clear();
    lastTransferTx.clear();
    lastMintTx.clear();
    lastTokenId.clear();
}

void Game::update(float dt) {
    fighter1->update(dt, window);
    fighter2->update(dt, window);

    if (fighter1->isAttacking()) {
        const bool hitboxOverlap = fighter1->getAttackHitbox().intersects(fighter2->getHitbox());
        const bool closeRange = fighter1->getHitbox().intersects(fighter2->getHitbox());
        if ((hitboxOverlap || closeRange) && !fighter2->isShielding() && !hit2ThisFrame) {
            fighter2->hurt(kDamagePerHit);
            hit2ThisFrame = true;
        }
    } else {
        hit2ThisFrame = false;
    }

    if (fighter2->isAttacking()) {
        const bool hitboxOverlap = fighter2->getAttackHitbox().intersects(fighter1->getHitbox());
        const bool closeRange = fighter2->getHitbox().intersects(fighter1->getHitbox());
        if ((hitboxOverlap || closeRange) && !fighter1->isShielding() && !hit1ThisFrame) {
            fighter1->hurt(kDamagePerHit);
            hit1ThisFrame = true;
        }
    } else {
        hit1ThisFrame = false;
    }

    if (fighter1->getHP() <= 0 || fighter2->getHP() <= 0) {
        running = false;
    }

    if (!running && !mintTriggered) {
        awaitingConfirmation = true;
    }
}

void Game::triggerMintIfNeeded() {
    mintTriggered = true;

    const int winner = getWinner();
    if (winner == 0) {
        statusTextString = "Match draw: no reward minted.";
        return;
    }

    const BridgeSettings bridge = loadBridgeSettings();
    const std::string mintUrl = mintNftUrl(bridge);

    const std::string player1Addr =
        g_player1_wallet.empty() ? "0x0000000000000000000000000000000000000001" : g_player1_wallet;
    const std::string player2Addr =
        g_player2_wallet.empty() ? "0x0000000000000000000000000000000000000002" : g_player2_wallet;
    const std::string winnerAddress = winner == 1 ? player1Addr : player2Addr;

    unsigned long long winnerCharacterId = 0;
    const std::string& winnerCharRaw = winner == 1 ? g_player1_character_token_id : g_player2_character_token_id;
    if (!winnerCharRaw.empty()) {
        try {
            winnerCharacterId = std::stoull(winnerCharRaw);
        } catch (...) {
            winnerCharacterId = 0;
        }
    }

    const auto now = std::chrono::system_clock::now();
    const auto nowMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

    std::ostringstream matchId;
    matchId << "match-" << nowMs;

    std::ostringstream payload;
    payload << "{"
            << "\"winner\":\"" << jsonEscape(winnerAddress) << "\","
            << "\"weaponName\":\"" << jsonEscape(bridge.defaultWeaponName) << "\","
            << "\"matchId\":\"" << jsonEscape(matchId.str()) << "\","
            << "\"player1\":\"" << jsonEscape(player1Addr) << "\","
            << "\"player2\":\"" << jsonEscape(player2Addr) << "\","
            << "\"winnerCharacterId\":" << winnerCharacterId << "}";

    statusTextString = "Settling battle on-chain...";

    postJsonAsync(mintUrl, payload.str(), [this](bool success, std::string resp) {
        if (!success) {
            statusTextString = "Bridge error: " + resp;
            return;
        }

        if (!jsonStatusSuccess(resp)) {
            statusTextString = "Mint failed: " + resp;
            return;
        }

        lastTransferTx = extractJsonString(resp, "battleTxHash").value_or("");
        lastMintTx = extractJsonString(resp, "mintTxHash").value_or("");
        lastTokenId = extractJsonString(resp, "tokenId").value_or("");
        mintSucceeded = !lastMintTx.empty();
        statusTextString = mintSucceeded ? "Battle settled and reward minted." : "Battle settled.";
    });
}

void Game::copyToClipboard(const std::string& text) {
#ifdef _WIN32
    if (text.empty() || !OpenClipboard(nullptr)) {
        return;
    }

    EmptyClipboard();
    HGLOBAL handle = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    if (!handle) {
        CloseClipboard();
        return;
    }

    void* destination = GlobalLock(handle);
    if (!destination) {
        GlobalFree(handle);
        CloseClipboard();
        return;
    }

    std::memcpy(destination, text.c_str(), text.size() + 1);
    GlobalUnlock(handle);
    SetClipboardData(CF_TEXT, handle);
    CloseClipboard();
#else
    (void)text;
#endif
}

void Game::handleMouseClick(const sf::Vector2f& mousePos) {
    if (awaitingConfirmation) {
        if (confirmButton.getGlobalBounds().contains(mousePos)) {
            awaitingConfirmation = false;
            triggerMintIfNeeded();
            return;
        }

        if (cancelButton.getGlobalBounds().contains(mousePos)) {
            awaitingConfirmation = false;
            mintTriggered = true;
            statusTextString = "Battle settlement cancelled.";
            return;
        }
    }

    if (!mintSucceeded) {
        return;
    }

    if (copyTransferButton.getGlobalBounds().contains(mousePos) && !lastTransferTx.empty()) {
        copyToClipboard(lastTransferTx);
        statusTextString = "Copied battle transaction hash.";
    } else if (copyMintButton.getGlobalBounds().contains(mousePos) && !lastMintTx.empty()) {
        copyToClipboard(lastMintTx);
        statusTextString = "Copied mint transaction hash.";
    } else if (copyTokenButton.getGlobalBounds().contains(mousePos) && !lastTokenId.empty()) {
        copyToClipboard(lastTokenId);
        statusTextString = "Copied token id.";
    }
}

void Game::render() {
    arena.draw(window);
    fighter1->draw(window);
    fighter2->draw(window);
    fighter1->drawHealthBar(window, true);
    fighter2->drawHealthBar(window, false);

    if (!statusTextString.empty()) {
        sf::Text statusText(statusTextString, statusFont, 24);
        statusText.setFillColor(sf::Color::White);
        statusText.setPosition(10.f, 10.f);
        window.draw(statusText);
    }

    if (awaitingConfirmation) {
        sf::RectangleShape overlay({static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)});
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(overlay);

        sf::Text info("Finalize the match on-chain and mint the champion reward?", statusFont, 20);
        info.setFillColor(sf::Color::White);
        info.setPosition(120.f, 200.f);
        window.draw(info);

        window.draw(confirmButton);
        window.draw(cancelButton);
        window.draw(confirmText);
        window.draw(cancelText);
    }

    if (mintSucceeded) {
        sf::RectangleShape overlay({static_cast<float>(window.getSize().x), static_cast<float>(window.getSize().y)});
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(overlay);

        sf::Text title("ON-CHAIN REWARD MINTED", statusFont, 32);
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(230.f, 120.f);
        window.draw(title);

        sf::Text battleTx("Battle tx: " + lastTransferTx, statusFont, 18);
        battleTx.setFillColor(sf::Color::White);
        battleTx.setPosition(60.f, 200.f);
        window.draw(battleTx);

        sf::Text mintTx("Mint tx: " + lastMintTx, statusFont, 18);
        mintTx.setFillColor(sf::Color::White);
        mintTx.setPosition(60.f, 240.f);
        window.draw(mintTx);

        sf::Text tokenId("Token ID: " + lastTokenId, statusFont, 18);
        tokenId.setFillColor(sf::Color::White);
        tokenId.setPosition(60.f, 280.f);
        window.draw(tokenId);

        window.draw(copyTransferButton);
        window.draw(copyMintButton);
        window.draw(copyTokenButton);
        window.draw(copyTransferText);
        window.draw(copyMintText);
        window.draw(copyTokenText);
    }
}

bool Game::isRunning() const {
    return running;
}

int Game::getWinner() const {
    if (fighter1->getHP() <= 0 && fighter2->getHP() <= 0) {
        return 0;
    }

    if (fighter1->getHP() <= 0) {
        return 2;
    }

    if (fighter2->getHP() <= 0) {
        return 1;
    }

    return 0;
}

void Game::setMapBackground(int mapIndex) {
    static const char* kMaps[] = {
        "Resources/Images/Background/arena.jpg",
        "Resources/Images/Background/desert.jpg",
        "Resources/Images/Background/forest.png"
    };

    if (mapIndex < 0 || mapIndex >= 3) {
        mapIndex = 0;
    }

    arena.setBackground(findResourcePath(kMaps[mapIndex]));
}
