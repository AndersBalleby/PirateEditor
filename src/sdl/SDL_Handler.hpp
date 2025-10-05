#pragma once
#include <string>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include "SDL3/SDL_keyboard.h"
#include "logging/Logger.hpp"
#include "core/ResourceManager.hpp"

struct WindowConfig {
  std::string title;
  int width;
  int height;
};

struct SDL_State {
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;
    TTF_Font*     font     = nullptr;
    bool          running  = false;
    float         deltaTime = 0.0f;
    float         windowHeight = 0.0f;
    float         windowWidth = 0.0f;
    const bool* keyState;

    void tickDeltaTime(uint64_t now, uint64_t last);
};

class SDL_Handler {
  public:
    SDL_Handler(WindowConfig winConfig);
    ~SDL_Handler();

    bool isRunning() const;
    void clear();
    void present();

    static SDL_State& getState();
    SDL_Renderer* getRenderer() const;
    SDL_Texture*  loadTexture(const std::string& path);
    TTF_Font*     loadFont(const std::string& path, size_t size);
    TTF_Font*     getFont() const;

    SDL_Handler(const SDL_Handler&) = delete;
    SDL_Handler& operator=(const SDL_Handler&) = delete;

  private:
    bool initSDL();
    void cleanup();

    static SDL_State state;
};
