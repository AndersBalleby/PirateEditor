#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "logging/Logger.hpp"
#include "ResourceManager.hpp"

struct WindowConfig {
  std::string title;
  int width;
  int height;
};

class SDL_Handler {
  public:
    SDL_Handler(WindowConfig winConfig);
    ~SDL_Handler();

    bool isRunning() const;
    void clear();
    void present();

    SDL_Renderer* getRenderer() const;
    SDL_Texture*  loadTexture(const std::string& path);
    TTF_Font*     loadFont(const std::string& path, size_t size);

    SDL_Handler(const SDL_Handler&) = delete;
    SDL_Handler& operator=(const SDL_Handler&) = delete;

  private:
    bool initSDL();
    void cleanup();

    SDL_Window* window     = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool running           = false;
};