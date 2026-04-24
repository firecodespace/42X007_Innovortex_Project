#pragma once

#include <string>

struct BridgeSettings {
    std::string baseUrl = "http://localhost:3000";
    std::string mintPath = "/mintNFT";
    std::string defaultWeaponName = "Champion Blade";
};

BridgeSettings loadBridgeSettings();

std::string joinUrl(const std::string& base, const std::string& path);

std::string mintNftUrl(const BridgeSettings& s);
