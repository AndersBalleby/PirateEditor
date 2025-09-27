#pragma once
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <string>
#include <unordered_map>

#include "logging/Logger.hpp"

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