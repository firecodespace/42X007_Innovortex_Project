#include <SFML/Graphics.hpp>
#include <imgui.h>
#include <imgui-SFML.h>
#include <unordered_map>
#include <memory>
#include <string>

struct ResourceCache {
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    sf::Texture& getTexture(const std::string& path) {
        if (auto it = textures.find(path); it != textures.end()) return *it->second;
        auto t = std::make_unique<sf::Texture>();
        if (!t->loadFromFile(path)) throw std::runtime_error("failed texture: " + path);
        t->setSmooth(true);
        auto& ref = *t; textures.emplace(path, std::move(t)); return ref;
    }
};

static void ApplyConsoleTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    s.WindowRounding = 0.f; s.FrameRounding = 0.f; s.ScrollbarRounding = 0.f;
    s.WindowPadding = ImVec2(28, 28); s.FramePadding = ImVec2(18, 14);
    s.ItemSpacing = ImVec2(14, 20); s.ItemInnerSpacing = ImVec2(10, 10);
    s.Colors[ImGuiCol_WindowBg] = ImVec4(0.08f, 0.06f, 0.05f, 0.96f);
    s.Colors[ImGuiCol_Button] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    s.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    s.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    s.Colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.90f, 1.00f);
    s.Colors[ImGuiCol_Border] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
}

enum MenuState { Main, Wallet, PurchaseTools, Upgrades };

int main() {
    ResourceCache cache;
    sf::Texture* bg = nullptr;
    try { bg = &cache.getTexture("assets/textures/arena.png"); }
    catch (...) {}
    unsigned w = bg ? bg->getSize().x : 1280, h = bg ? bg->getSize().y : 720;

    sf::RenderWindow window(sf::VideoMode(w, h), "Gladiator Arena", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);
    ImGui::SFML::Init(window);
    ApplyConsoleTheme();

    ImGuiIO& io = ImGui::GetIO();
    ImFont* titleFont = io.Fonts->AddFontFromFileTTF("assets/fonts/ARCADECLASSIC.TTF", 42.0f);
    ImFont* itemFont = io.Fonts->AddFontFromFileTTF("assets/fonts/ARCADECLASSIC.TTF", 30.0f);
    ImFont* smallFont = io.Fonts->AddFontFromFileTTF("assets/fonts/ARCADECLASSIC.TTF", 22.0f);
    ImGui::SFML::UpdateFontTexture();

    sf::Sprite bgSprite; if (bg) bgSprite.setTexture(*bg);
    sf::Clock clk;

    bool menuOpen = false, escPressed = false;
    MenuState state = Main;
    int selection = 0;

    // Game state
    bool walletConnected = false;
    int gold = 250;
    bool swordOwned = false, shieldOwned = false;
    int dmgLvl = 0, hpLvl = 0, spdLvl = 0;

    auto DrawCentered = [](ImFont* font, const char* text, float menuW) {
        ImGui::PushFont(font);
        float tw = ImGui::CalcTextSize(text).x;
        ImGui::SetCursorPosX((menuW - tw) * 0.5f);
        ImGui::Text("%s", text);
        ImGui::PopFont();
        };

    auto MenuItem = [&](const char* label, bool selected, float menuW) {
        ImVec4 col = selected ? ImVec4(1.0f, 0.85f, 0.30f, 1.0f) : ImVec4(0.95f, 0.95f, 0.90f, 1.0f);
        ImGui::PushStyleColor(ImGuiCol_Text, col);
        ImGui::PushFont(itemFont);
        float tw = ImGui::CalcTextSize(label).x;
        ImGui::SetCursorPosX((menuW - tw) * 0.5f);
        bool pressed = ImGui::Selectable(label, selected, ImGuiSelectableFlags_None, ImVec2(tw, 0));
        ImGui::PopFont();
        ImGui::PopStyleColor();
        ImGui::Spacing();
        return pressed;
        };

    sf::Event e;
    while (window.isOpen()) {
        while (window.pollEvent(e)) {
            if (e.type == sf::Event::Closed) window.close();
            ImGui::SFML::ProcessEvent(window, e);
        }

        bool escNow = sf::Keyboard::isKeyPressed(sf::Keyboard::Escape);
        if (escNow && !escPressed) {
            if (state == Main) menuOpen = !menuOpen;
            else { state = Main; selection = 0; }
        }
        escPressed = escNow;

        ImGui::SFML::Update(window, clk.restart());

        if (menuOpen) {
            float menuW = 540.f, menuH = 500.f;
            ImGui::SetNextWindowPos(ImVec2((w - menuW) * 0.5f, (h - menuH) * 0.5f), ImGuiCond_Always);
            ImGui::SetNextWindowSize(ImVec2(menuW, menuH), ImGuiCond_Always);
            ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);

            if (state == Main) {
                DrawCentered(titleFont, "MAIN  MENU", menuW);
                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();
                if (MenuItem("WALLET", selection == 0, menuW)) { state = Wallet; selection = 0; }
                if (MenuItem("PURCHASE  TOOLS", selection == 1, menuW)) { state = PurchaseTools; selection = 0; }
                if (MenuItem("UPGRADES", selection == 2, menuW)) { state = Upgrades; selection = 0; }
                if (MenuItem("EXIT", selection == 3, menuW)) window.close();
            }
            else if (state == Wallet) {
                DrawCentered(titleFont, "WALLET", menuW);
                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::PushFont(smallFont);
                ImGui::Text("Status    %s", walletConnected ? "connected" : "not  connected");
                ImGui::Spacing(); ImGui::Spacing();
                float btnW = 360.f;
                ImGui::SetCursorPosX((menuW - btnW) * 0.5f);
                if (ImGui::Button(walletConnected ? "Disconnect" : "Connect", ImVec2(btnW, 50)))
                    walletConnected = !walletConnected;
                ImGui::Spacing(); ImGui::Spacing();
                ImGui::Separator(); ImGui::Spacing();
                ImGui::Text("Press  ESC  to  return");
                ImGui::PopFont();
            }
            else if (state == PurchaseTools) {
                DrawCentered(titleFont, "PURCHASE  TOOLS", menuW);
                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::PushFont(smallFont);
                ImGui::Text("Gold    %d", gold);
                ImGui::Spacing(); ImGui::Spacing();
                float btnW = 360.f;
                ImGui::SetCursorPosX((menuW - btnW) * 0.5f);
                if (!swordOwned) {
                    if (ImGui::Button("Buy  Sword    80g", ImVec2(btnW, 50)) && gold >= 80) { gold -= 80; swordOwned = true; }
                }
                else {
                    ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Sword    Owned");
                }
                ImGui::Spacing();
                ImGui::SetCursorPosX((menuW - btnW) * 0.5f);
                if (!shieldOwned) {
                    if (ImGui::Button("Buy  Shield    60g", ImVec2(btnW, 50)) && gold >= 60) { gold -= 60; shieldOwned = true; }
                }
                else {
                    ImGui::TextColored(ImVec4(0.3f, 1.0f, 0.3f, 1.0f), "Shield    Owned");
                }
                ImGui::Spacing(); ImGui::Spacing();
                ImGui::Separator(); ImGui::Spacing();
                ImGui::Text("Press  ESC  to  return");
                ImGui::PopFont();
            }
            else if (state == Upgrades) {
                DrawCentered(titleFont, "UPGRADES", menuW);
                ImGui::Spacing(); ImGui::Separator(); ImGui::Spacing(); ImGui::Spacing();
                ImGui::PushFont(smallFont);
                ImGui::Text("Gold    %d", gold);
                ImGui::Spacing(); ImGui::Spacing();
                float btnW = 360.f;
                ImGui::SetCursorPosX((menuW - btnW) * 0.5f);
                if (ImGui::Button("Damage  +1    50g", ImVec2(btnW, 50)) && gold >= 50) { gold -= 50; dmgLvl++; }
                ImGui::Spacing();
                ImGui::SetCursorPosX((menuW - btnW) * 0.5f);
                if (ImGui::Button("Health  +10    50g", ImVec2(btnW, 50)) && gold >= 50) { gold -= 50; hpLvl++; }
                ImGui::Spacing();
                ImGui::SetCursorPosX((menuW - btnW) * 0.5f);
                if (ImGui::Button("Speed  +1    40g", ImVec2(btnW, 50)) && gold >= 40) { gold -= 40; spdLvl++; }
                ImGui::Spacing(); ImGui::Spacing();
                ImGui::Separator(); ImGui::Spacing();
                ImGui::Text("Levels    D %d    H %d    S %d", dmgLvl, hpLvl, spdLvl);
                ImGui::Spacing();
                ImGui::Text("Press  ESC  to  return");
                ImGui::PopFont();
            }

            ImGui::End();
        }

        window.clear(sf::Color(10, 8, 6));
        if (bg) window.draw(bgSprite);
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}
