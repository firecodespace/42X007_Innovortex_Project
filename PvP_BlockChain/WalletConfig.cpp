#include "WalletConfig.h"
#include <fstream>
#include <sstream>
namespace WalletConfig {
    static std::string p1, p2;
    static bool loaded = false;

    static void ensureLoaded() {
        if (loaded) return;
        loaded = true;
        std::ifstream in("Wallets.json");
        if (!in) return;
        std::string line;
        while (std::getline(in, line)) {
            auto pos1 = line.find("\"player1\"");
            if (pos1 != std::string::npos) {
                auto colon = line.find(':', pos1);
                if (colon != std::string::npos) {
                    auto start = line.find('"', colon+1);
                    if (start != std::string::npos) {
                        auto end = line.find('"', start+1);
                        if (end != std::string::npos) p1 = line.substr(start+1, end-start-1);
                    }
                }
            }
            auto pos2 = line.find("\"player2\"");
            if (pos2 != std::string::npos) {
                auto colon = line.find(':', pos2);
                if (colon != std::string::npos) {
                    auto start = line.find('"', colon+1);
                    if (start != std::string::npos) {
                        auto end = line.find('"', start+1);
                        if (end != std::string::npos) p2 = line.substr(start+1, end-start-1);
                    }
                }
            }
        }
    }

    std::string getPlayerWallet(int player) {
        ensureLoaded();
        return (player == 1) ? p1 : p2;
    }
}
