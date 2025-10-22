#pragma once

#include <string>
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "logging/Logger.hpp"
#include "scene/Scene.hpp"
#include "sdl/SDL_Handler.hpp"
#include "ui/TextHandler.hpp"

class Editor {
  public:
    Editor();
    ~Editor() = default;

    void run(SDL_State& state);
    void update(SDL_State& state);
    void draw(SDL_State& state);
    void handleInput(SDL_Event& event);

  private:
    void drawGridLines(SDL_State& state);
    void updateSelectedTiles(SDL_State& state);

    static constexpr float mapHeight = 704.0f;
    static constexpr SDL_Color gridColor = {120, 135, 150, 255};
    static constexpr SDL_Color hoverColor = {30, 150, 255, 255};
    static constexpr SDL_Color testColor = {255, 165, 0, 255};
    float mouseX = 0.0f;
    float mouseY = 0.0f;
    float mapOffsetY = 0.0f;
    bool editMode = true;
    bool showLayers = false;

    int currentLayer = 0;
    const int maxLayers = 3;

    bool isSelecting = false;
    bool wasMouseDown = false;
    SDL_FPoint selectionStart{0.0f, 0.0f};
    SDL_FPoint selectionEnd{0.0f, 0.0f};
    std::vector<std::pair<int, int>> selectedTiles;

    Scene::Manager scene_manager;
};
