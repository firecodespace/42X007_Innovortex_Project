#include "Game.h"
#include "http_client.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <iomanip>
#include "Wallets.h"
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

using json = nlohmann::json;

Game::Game(sf::RenderWindow& win)
    : window(win),
    arena("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/Background/arena.jpg", win.getSize()),
    fighter1(nullptr),
    fighter2(nullptr),
    running(true),
    hit1ThisFrame(false),
    hit2ThisFrame(false)
{
    if (!music.openFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Music/retro-arcade-game-music-297305.mp3")) {
        std::cout << "ERROR: Failed to load background music" << std::endl;
    }
    music.setLoop(true);
    music.setVolume(50);
    music.play();

    reset();

    // load font for status messages
    if (!statusFont.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/fonts/ARCADECLASSIC.TTF")) {
        std::cout << "WARN: failed to load status font" << std::endl;
    }

    // prepare confirm/cancel buttons
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

    // post-mint copy buttons
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
    copyTransferText.setString("COPY TRANSFER TX");
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
}

void Game::reset() {
    if (fighter1) delete fighter1;
    if (fighter2) delete fighter2;

    fighter1 = new Fighter(
        200.f, window.getSize().y - 128.f * 2.0f - 12,
        sf::Keyboard::A, sf::Keyboard::D,
        sf::Keyboard::F, sf::Keyboard::G, sf::Keyboard::H,
        sf::Keyboard::W, sf::Keyboard::Q);

    fighter2 = new Fighter(
        700.f, window.getSize().y - 128.f * 2.0f - 12,
        sf::Keyboard::Left, sf::Keyboard::Right,
        sf::Keyboard::Numpad1, sf::Keyboard::Numpad2, sf::Keyboard::Numpad3,
        sf::Keyboard::Up, sf::Keyboard::RControl);

    running = true;
    hit1ThisFrame = false;
    hit2ThisFrame = false;
}

void Game::update(float dt) {
    fighter1->update(dt, window);
    fighter2->update(dt, window);

    // Fighter 1 attacking Fighter 2
    if (fighter1->isAttacking()) {
        // Check BOTH attack hitbox AND body overlap for close range
        bool hitboxOverlap = fighter1->getAttackHitbox().intersects(fighter2->getHitbox());
        bool closeRange = fighter1->getHitbox().intersects(fighter2->getHitbox());

        if ((hitboxOverlap || closeRange) && !fighter2->isShielding() && !hit2ThisFrame) {
            fighter2->hurt(10);
            hit2ThisFrame = true;
        }
    }
    else {
        hit2ThisFrame = false;
    }

    // Fighter 2 attacking Fighter 1
    if (fighter2->isAttacking()) {
        // Check BOTH attack hitbox AND body overlap for close range
        bool hitboxOverlap = fighter2->getAttackHitbox().intersects(fighter1->getHitbox());
        bool closeRange = fighter2->getHitbox().intersects(fighter1->getHitbox());

        if ((hitboxOverlap || closeRange) && !fighter1->isShielding() && !hit1ThisFrame) {
            fighter1->hurt(10);
            hit1ThisFrame = true;
        }
    }
    else {
        hit1ThisFrame = false;
    }

    if (fighter1->getHP() <= 0 || fighter2->getHP() <= 0)
        running = false;

    // After match ends, trigger mint once
    if (!running && !mintTriggered) {
        // show confirmation overlay instead of immediate trigger
        awaitingConfirmation = true;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        running = false;
}

// Build JSON and POST to Node bridge
void Game::triggerMintIfNeeded() {
    mintTriggered = true;

    int winner = getWinner();
    if (winner == 0) {
        statusTextString = "Match draw: no NFT minted.";
        return;
    }

    // In a real game you'd query the player's real wallet; here we use placeholder addresses
    std::string winnerAddress = (winner == 1) ? "0xWinnerAddress1" : "0xWinnerAddress2";
    std::string weaponName = "Sword_02"; // example, could be from fighter state

    // Create a UUID-like matchId (timestamp-based)
    auto now = std::chrono::system_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::ostringstream oss;
    oss << "match-" << now_ms;
    std::string matchId = oss.str();

    // Use stored player wallet addresses if available, otherwise placeholders
    std::string player1Addr = !player1Wallet.empty() ? player1Wallet : "0xPlayer1Placeholder";
    std::string player2Addr = !player2Wallet.empty() ? player2Wallet : "0xPlayer2Placeholder";

    json payload = {
        {"winner", winnerAddress},
        {"weaponName", weaponName},
        {"matchId", matchId},
        {"player1", player1Addr},
        {"player2", player2Addr}
    };

    statusTextString = "Minting NFT...";

    postJsonAsync("http://localhost:3000/mintNFT", payload.dump(), [this](bool success, std::string resp) {
        if (success) {
            // parse out txHash if present
            try {
                auto j = json::parse(resp);
                if (j.contains("status") && j["status"] == "success") {
                    std::string tx = j.value("txHash", "");
                    statusTextString = std::string("Transaction Successful! tx: ") + tx;
                    // populate post-mint UI fields
                    lastMintTx = tx;
                    lastTransferTx = j.value("transferTxHash", "");
                    lastTokenId = j.value("tokenId", "");
                    mintSucceeded = true;
                } else {
                    statusTextString = std::string("Mint failed: ") + resp;
                }
            } catch (...) {
                statusTextString = std::string("Mint response: ") + resp;
            }
        } else {
            statusTextString = std::string("Mint error: ") + resp;
        }
    });
}

// Handle mouse clicks for confirmation
// Call this from GamePlayState event polling if you want; for simplicity we poll global events in Game->update
// We'll add a small check in Game::render to read mouse button state (polling) — for production use proper event handling.
// For now provide a public method used by GamePlayState to forward events if needed.

void Game::handleMouseClick(const sf::Vector2f& mousePos) {
    if (!awaitingConfirmation) return;
    if (confirmButton.getGlobalBounds().contains(mousePos)) {
        awaitingConfirmation = false;
        statusTextString = "Minting NFT...";
        triggerMintIfNeeded();
    } else if (cancelButton.getGlobalBounds().contains(mousePos)) {
        awaitingConfirmation = false;
        statusTextString = "Mint cancelled by player";
    }

    // If mint succeeded, handle copy button clicks
    if (mintSucceeded) {
        if (copyTransferButton.getGlobalBounds().contains(mousePos) && !lastTransferTx.empty()) {
            // copy to clipboard
#ifdef _WIN32
            OpenClipboard(NULL);
            EmptyClipboard();
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, lastTransferTx.size() + 1);
            memcpy(GlobalLock(hMem), lastTransferTx.c_str(), lastTransferTx.size() + 1);
            GlobalUnlock(hMem);
            SetClipboardData(CF_TEXT, hMem);
            CloseClipboard();
#endif
            statusTextString = "Copied transfer tx to clipboard";
        }
        if (copyMintButton.getGlobalBounds().contains(mousePos) && !lastMintTx.empty()) {
#ifdef _WIN32
            OpenClipboard(NULL);
            EmptyClipboard();
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, lastMintTx.size() + 1);
            memcpy(GlobalLock(hMem), lastMintTx.c_str(), lastMintTx.size() + 1);
            GlobalUnlock(hMem);
            SetClipboardData(CF_TEXT, hMem);
            CloseClipboard();
#endif
            statusTextString = "Copied mint tx to clipboard";
        }
        if (copyTokenButton.getGlobalBounds().contains(mousePos) && !lastTokenId.empty()) {
#ifdef _WIN32
            OpenClipboard(NULL);
            EmptyClipboard();
            HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, lastTokenId.size() + 1);
            memcpy(GlobalLock(hMem), lastTokenId.c_str(), lastTokenId.size() + 1);
            GlobalUnlock(hMem);
            SetClipboardData(CF_TEXT, hMem);
            CloseClipboard();
#endif
            statusTextString = "Copied token id to clipboard";
        }
    }
}


void Game::render() {
    arena.draw(window);
    fighter1->draw(window);
    fighter2->draw(window);
    fighter1->drawHealthBar(window, true);
    fighter2->drawHealthBar(window, false);

    // Draw status text (minting progress / result)
    if (!statusTextString.empty()) {
        sf::Text statusText;
        statusText.setFont(statusFont);
        statusText.setString(statusTextString);
        statusText.setCharacterSize(24);
        statusText.setFillColor(sf::Color::White);
        statusText.setPosition(10.f, 10.f);
        window.draw(statusText);
    }

    // Confirmation overlay
    if (awaitingConfirmation) {
        sf::RectangleShape overlay(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(overlay);

        sf::Text info("Confirm transfer: 30 ETH will be sent from loser to winner. Proceed?", statusFont, 20);
        info.setFillColor(sf::Color::White);
        info.setPosition(120.f, 200.f);
        window.draw(info);

        window.draw(confirmButton);
        window.draw(cancelButton);
        window.draw(confirmText);
        window.draw(cancelText);
    }

    // Post-mint results
    if (mintSucceeded) {
        sf::RectangleShape overlay(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
        overlay.setFillColor(sf::Color(0, 0, 0, 200));
        window.draw(overlay);

        sf::Text title("MINT SUCCESS", statusFont, 36);
        title.setFillColor(sf::Color::Yellow);
        title.setPosition(340.f, 120.f);
        window.draw(title);

        sf::Text t1(std::string("Transfer tx: ") + lastTransferTx, statusFont, 18);
        t1.setFillColor(sf::Color::White);
        t1.setPosition(60.f, 200.f);
        window.draw(t1);

        sf::Text t2(std::string("Mint tx: ") + lastMintTx, statusFont, 18);
        t2.setFillColor(sf::Color::White);
        t2.setPosition(60.f, 240.f);
        window.draw(t2);

        sf::Text t3(std::string("Token ID: ") + lastTokenId, statusFont, 18);
        t3.setFillColor(sf::Color::White);
        t3.setPosition(60.f, 280.f);
        window.draw(t3);

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
    if (fighter1->getHP() <= 0 && fighter2->getHP() <= 0) return 0;
    if (fighter1->getHP() <= 0) return 2;
    if (fighter2->getHP() <= 0) return 1;
    return 0;
}
