#include "BridgeConfig.h"

#include "ResourcePaths.h"
#include "json_util.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

namespace {

std::string trim(std::string s) {
    auto notSpace = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

std::string readFileIfExists(const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) {
        return {};
    }
    std::ostringstream buf;
    buf << in.rdbuf();
    return buf.str();
}

}  // namespace

std::string joinUrl(const std::string& base, const std::string& path) {
    if (base.empty()) {
        return path;
    }
    if (!path.empty() && path[0] == '/') {
        if (!base.empty() && base.back() == '/') {
            return base.substr(0, base.size() - 1) + path;
        }
        return base + path;
    }
    if (!base.empty() && base.back() != '/') {
        return base + "/" + path;
    }
    return base + path;
}

BridgeSettings loadBridgeSettings() {
    BridgeSettings out;
    const std::string path = findResourcePath("Resources/bridge_config.json");
    const std::string raw = readFileIfExists(path);
    if (raw.empty()) {
        return out;
    }

    if (const auto v = extractJsonString(raw, "baseUrl")) {
        out.baseUrl = trim(*v);
    }
    if (const auto v = extractJsonString(raw, "mintPath")) {
        out.mintPath = trim(*v);
        if (!out.mintPath.empty() && out.mintPath[0] != '/') {
            out.mintPath = "/" + out.mintPath;
        }
    }
    if (const auto v = extractJsonString(raw, "weaponName")) {
        out.defaultWeaponName = trim(*v);
    }
    return out;
}

std::string mintNftUrl(const BridgeSettings& s) {
    return joinUrl(s.baseUrl, s.mintPath.empty() ? std::string("/mintNFT") : s.mintPath);
}
