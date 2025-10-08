#pragma once

#include "SDL3/SDL_pixels.h"
#include "logging/Logger.hpp"
#include "scene/Scene.hpp"
#include "sdl/SDL_Handler.hpp"

class Editor {
  public:
    Editor();
    ~Editor() = default;

    void run(SDL_State& state);
    void update(SDL_State& state);
    void draw(SDL_State& state);


  private:
    void drawGridLines(SDL_State& state);

    static constexpr float mapHeight = 704.0f;
    static constexpr SDL_Color gridColor = {120, 135, 150, 255};
    static constexpr SDL_Color hoverColor = {30, 150, 255, 255};
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    float mapOffsetY = 0.0f;

    Scene::Manager scene_manager;
};
