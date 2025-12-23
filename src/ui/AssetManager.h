#ifndef ASSET_MANAGER_H
#define ASSET_MANAGER_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <memory>

namespace DrumMachine {

/**
 * AssetManager
 * 
 * Loads and manages custom graphical assets.
 * Textures, fonts, and graphics resources.
 * Milestone 4: Custom asset integration with ImGui
 */
class AssetManager {
public:
    AssetManager(const std::string& assetBasePath = "assets/");
    ~AssetManager();

    // Load texture from file
    bool loadTexture(const std::string& name, const std::string& filePath);

    // Load font from file
    bool loadFont(const std::string& name, const std::string& filePath, float size);

    // Get texture handle (for ImGui)
    void* getTexture(const std::string& name) const;

    // Get font handle (for ImGui)
    void* getFont(const std::string& name) const;

private:
    std::string assetBasePath_;
    std::unordered_map<std::string, void*> textures_;    // ImGui texture IDs
    std::unordered_map<std::string, void*> fonts_;       // ImFont pointers
};

} // namespace DrumMachine

#endif // ASSET_MANAGER_H
