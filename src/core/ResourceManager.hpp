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
#include "math/vec.hpp"

const int TILE_SIZE = 64;
class Animation {
  public:
    Animation(const std::string& animID, const std::filesystem::path& folderPath);
    ~Animation() = default; // ResourceManager sørger for at destroy alle textures

    /* Funktionalitet */
    void tick(float deltaTime);
    void draw(SDL_Renderer* renderer, SDL_FRect* srcRect, SDL_FRect* destRect);

    /* Getters */
    float getCurrentFrame() const;
    std::vector<SDL_Texture*> getTextures();
    SDL_Texture* getCurrentTexture() const;
    int getHeight() const;
    int getWidth() const;
  private:
    float ANIMATION_SPEED = 10.0f;
    std::string animID;
    std::vector<SDL_Texture*> textures;
    float current_frame;
};

class ResourceManager {
  public:
    /* Husk at kalde :D */
    static bool init(SDL_Renderer* renderer);

    static SDL_Texture* loadTexture(const std::string& path);
    static SDL_Texture* loadTileMap(const std::string& path);

    /* Wrapper for Animation constructor */
    static Animation* loadAnimation(const std::string& animID, const std::filesystem::path& folderPath);
    static Animation* getAnimation(const std::string& animID);

    // Clear alle resources
    static void clear();

    /* Opdaterer alle animations */
    static void update(float deltaTime);

    ResourceManager(const ResourceManager&) = delete;
    ResourceManager& operator=(const ResourceManager&) = delete;
  private:
    ResourceManager() = default;
    ~ResourceManager() = delete;

    static SDL_Renderer* s_renderer;
    static std::unordered_map<std::string, SDL_Texture*> s_textures;
    static std::unordered_map<std::string, std::unique_ptr<Animation>> s_animations;
};
