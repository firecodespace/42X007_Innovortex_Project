#include "ResourcePaths.h"

#include <filesystem>
#include <vector>

std::string findResourcePath(const std::string& relativePath) {
    namespace fs = std::filesystem;

    const std::vector<fs::path> roots = {
        fs::current_path(),
        fs::current_path() / "PvP_BlockChain",
        fs::current_path().parent_path(),
        fs::current_path().parent_path() / "PvP_BlockChain",
        fs::current_path().parent_path().parent_path(),
        fs::current_path().parent_path().parent_path() / "PvP_BlockChain"
    };

    for (const auto& root : roots) {
        const fs::path candidate = root / relativePath;
        if (fs::exists(candidate)) {
            return candidate.lexically_normal().string();
        }
    }

    return relativePath;
}
