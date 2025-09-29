#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <vector>
#include <unordered_map>
#include <filesystem>
#include <iostream>
#include <algorithm>

#include "logging/Logger.hpp"
#include "io/utils.hpp"

class Animation {
  public:
    Animation(const std::string& animID, const std::filesystem::path& folderPath);
    ~Animation() = default; // ResourceManager sørger for at destroy alle textures

    void tick(float deltaTime);
    
    float getCurrentFrame() const;
    std::vector<SDL_Texture*> getTextures();
  private:
    const float ANIMATION_SPEED = 10.0f;
    std::string animID;
    std::vector<SDL_Texture*> textures;
    float current_frame;
};

class ResourceManager {
  public:
    /* Husk at kalde :D */
    static bool init(SDL_Renderer* renderer);
    
    static SDL_Texture* loadTexture(const std::string& path);

    // Clear alle resources
    static void clear();

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
  private:
    ResourceManager() = default;
    ~ResourceManager() = delete;

    static SDL_Renderer* s_renderer;
    static std::unordered_map<std::string, SDL_Texture*> s_textures;
};