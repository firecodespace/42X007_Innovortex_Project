#pragma once

#include <string>
#include <functional>

// Simple async POST helper using libcurl. Callback signature: (success, responseBodyOrError)
void postJsonAsync(const std::string& url, const std::string& jsonBody, std::function<void(bool,std::string)> callback);
