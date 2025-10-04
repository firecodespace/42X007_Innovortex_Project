#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <memory>
#include <string>

class ResourceCache {
public:
    sf::Texture& getTexture(const std::string& path) {
        auto it = textures.find(path);
        if (it != textures.end()) return *it->second;
        auto tex = std::make_unique<sf::Texture>();
        tex->loadFromFile(path);
        tex->setSmooth(true);
        auto& ref = *tex;
        textures.emplace(path, std::move(tex));
        return ref;
    }
    sf::Font& getFont(const std::string& path) {
        auto it = fonts.find(path);
        if (it != fonts.end()) return *it->second;
        auto f = std::make_unique<sf::Font>();
        f->loadFromFile(path);
        auto& ref = *f;
        fonts.emplace(path, std::move(f));
        return ref;
    }
private:
    std::unordered_map<std::string, std::unique_ptr<sf::Texture>> textures;
    std::unordered_map<std::string, std::unique_ptr<sf::Font>> fonts;
};
