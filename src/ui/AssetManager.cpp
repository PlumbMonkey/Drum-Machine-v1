#include "AssetManager.h"
#include <iostream>

namespace DrumMachine {

AssetManager::AssetManager(const std::string& assetBasePath)
    : assetBasePath_(assetBasePath)
{
}

AssetManager::~AssetManager()
{
    // TODO: Cleanup textures and fonts
}

bool AssetManager::loadTexture(const std::string& name, const std::string& filePath)
{
    // TODO: Load texture using OpenGL/SDL2 image loading
    // - Load image file
    // - Create OpenGL texture
    // - Store texture ID in textures_ map
    
    std::cout << "Loading texture: " << name << " from " << filePath << std::endl;
    return true;
}

bool AssetManager::loadFont(const std::string& name, const std::string& filePath, float size)
{
    // TODO: Load font using ImGui
    // - Load .ttf file
    // - Add to ImGui font atlas
    // - Store ImFont* in fonts_ map
    
    std::cout << "Loading font: " << name << " from " << filePath << " at " << size << "px" << std::endl;
    return true;
}

void* AssetManager::getTexture(const std::string& name) const
{
    auto it = textures_.find(name);
    return (it != textures_.end()) ? it->second : nullptr;
}

void* AssetManager::getFont(const std::string& name) const
{
    auto it = fonts_.find(name);
    return (it != fonts_.end()) ? it->second : nullptr;
}

} // namespace DrumMachine
