#include "json_util.h"

#include <optional>
#include <string>

std::string jsonEscape(const std::string& value) {
    std::string escaped;
    escaped.reserve(value.size() + 8);
    for (char ch : value) {
        switch (ch) {
        case '\\':
            escaped += "\\\\";
            break;
        case '"':
            escaped += "\\\"";
            break;
        case '\n':
            escaped += "\\n";
            break;
        case '\r':
            escaped += "\\r";
            break;
        case '\t':
            escaped += "\\t";
            break;
        default:
            escaped += ch;
            break;
        }
    }
    return escaped;
}

std::optional<std::string> extractJsonString(const std::string& body, const std::string& key) {
    const std::string needle = "\"" + key + "\"";
    const std::size_t keyPos = body.find(needle);
    if (keyPos == std::string::npos) {
        return std::nullopt;
    }

    const std::size_t colonPos = body.find(':', keyPos + needle.size());
    if (colonPos == std::string::npos) {
        return std::nullopt;
    }

    const std::size_t firstQuote = body.find('"', colonPos + 1);
    if (firstQuote == std::string::npos) {
        return std::nullopt;
    }

    std::string result;
    bool escaped = false;
    for (std::size_t i = firstQuote + 1; i < body.size(); ++i) {
        const char ch = body[i];
        if (escaped) {
            result += ch;
            escaped = false;
        } else if (ch == '\\') {
            escaped = true;
        } else if (ch == '"') {
            return result;
        } else {
            result += ch;
        }
    }

    return std::nullopt;
}

bool jsonStatusSuccess(const std::string& body) {
    const auto status = extractJsonString(body, "status");
    return status.has_value() && status.value() == "success";
}
