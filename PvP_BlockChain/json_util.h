#pragma once

#include <optional>
#include <string>

std::optional<std::string> extractJsonString(const std::string& body, const std::string& key);
bool jsonStatusSuccess(const std::string& body);
std::string jsonEscape(const std::string& value);
