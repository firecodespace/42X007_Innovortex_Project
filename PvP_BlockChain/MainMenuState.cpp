#include "MainMenuState.h"
#include "Wallets.h"

MainMenuState::MainMenuState(sf::RenderWindow& win)
    : window(win),
    arena("Resources/Images/Video/ezgif-frame-001.jpg", win.getSize())
{
    font.loadFromFile("D:/amity/testing/2nd/PvP_BlockChain/PvP_BlockChain/Resources/Images/fonts/ARCADECLASSIC.TTF");

    // Load and play background music
    if (music.openFromFile("Resources/Music/retro-arcade-game-music-297305.mp3")) {
        music.setLoop(true);
        music.setVolume(50);
        music.play();
    }

    std::string labels[] = { "NEW GAME", "SELECT MAP", "STORE", "EXIT" };
    float startX = 40.f;
    float startY = window.getSize().y - 240.f;
    float spacing = 60.f;
    for (int i = 0; i < 4; ++i) {
        sf::Text t;
        initText(t, labels[i], startX, startY + spacing * i);
        buttons.push_back(t);
    }

    // Setup input text boxes
    inputText1.setFont(font);
    inputText1.setCharacterSize(20);
    inputText1.setFillColor(sf::Color::White);
    inputText1.setPosition(40.f, startY - 120.f);
    inputText1.setString("Player1 Wallet: ");

    inputText2.setFont(font);
    inputText2.setCharacterSize(20);
    inputText2.setFillColor(sf::Color::White);
    inputText2.setPosition(40.f, startY - 80.f);
    inputText2.setString("Player2 Wallet: ");
}

void MainMenuState::initText(sf::Text& text, const std::string& str, float x, float y) {
    text.setFont(font);
    text.setString(str);
    text.setCharacterSize(36);
    text.setFillColor(str == "NEW GAME" ? sf::Color(255, 215, 0) : sf::Color::White);
    text.setPosition(x, y);
}

StateID MainMenuState::update(float dt) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed)
            return StateID::Exit;
        if (event.type == sf::Event::MouseButtonPressed) {
            auto pos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            for (size_t i = 0; i < buttons.size(); ++i) {
                if (buttons[i].getGlobalBounds().contains(pos)) {
                    music.stop();  // Stop music when leaving menu
                    switch (i) {
                    case 0: {
                        // Before starting game, set global wallets
                        g_player1_wallet = inputPlayer1.empty() ? "0xPlayer1Placeholder" : inputPlayer1;
                        g_player2_wallet = inputPlayer2.empty() ? "0xPlayer2Placeholder" : inputPlayer2;
                        return StateID::GamePlay;
                    }
                    case 1: return StateID::MapSelection;
                    case 2: return StateID::Store;
                    case 3: return StateID::Exit;
                    }
                }
            }

            // click on input areas
            if (inputText1.getGlobalBounds().contains(pos)) {
                inputActive1 = true; inputActive2 = false;
            } else if (inputText2.getGlobalBounds().contains(pos)) {
                inputActive2 = true; inputActive1 = false;
            } else {
                inputActive1 = inputActive2 = false;
            }
        }

        // Text entered handling
        if (event.type == sf::Event::TextEntered) {
            auto unicode = event.text.unicode;
            if (unicode == 8) { // backspace
                if (inputActive1 && !inputPlayer1.empty()) inputPlayer1.pop_back();
                if (inputActive2 && !inputPlayer2.empty()) inputPlayer2.pop_back();
            } else if (unicode < 128) {
                char c = static_cast<char>(unicode);
                if (inputActive1) inputPlayer1.push_back(c);
                if (inputActive2) inputPlayer2.push_back(c);
            }
        }
    }
    return StateID::MainMenu;
}

void MainMenuState::render() {
    window.clear();
    arena.draw(window);

    sf::RectangleShape overlay(sf::Vector2f((float)window.getSize().x, (float)window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);

    sf::Text title("PIXEL PVP ARENA", font, 48);
    title.setFillColor(sf::Color::White);
    sf::FloatRect titleBound = title.getLocalBounds();
    title.setOrigin(titleBound.width / 2, titleBound.height / 2);
    title.setPosition(window.getSize().x / 2.f, 100.f);
    window.draw(title);

    for (auto& btn : buttons)
        window.draw(btn);

    // Draw input fields
    sf::Text label1 = inputText1;
    label1.setString(std::string("Player1 Wallet: ") + inputPlayer1 + (inputActive1 ? "|" : ""));
    window.draw(label1);

    sf::Text label2 = inputText2;
    label2.setString(std::string("Player2 Wallet: ") + inputPlayer2 + (inputActive2 ? "|" : ""));
    window.draw(label2);

    window.display();
}
