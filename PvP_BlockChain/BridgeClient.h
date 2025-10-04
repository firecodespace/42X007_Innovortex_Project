#pragma once
#include <string>

// Simple Windows HTTP client that POSTs a JSON payload to the bridge server
// Uses WinHTTP (Windows only). Link with winhttp.lib in your project.
namespace BridgeClient {
    // Post a win event to the bridge. Sends both winner and loser addresses. Returns true on success.
    bool postWin(const std::string& winnerAddress, const std::string& loserAddress, std::string& outResponse);
}
