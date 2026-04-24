#pragma once

#include <string>
#include <functional>

// Async HTTP helpers (WinHTTP on Windows). Callback: (success, responseBodyOrError)
void postJsonAsync(const std::string& url, const std::string& jsonBody, std::function<void(bool, std::string)> callback);
void getUrlAsync(const std::string& url, std::function<void(bool, std::string)> callback);
