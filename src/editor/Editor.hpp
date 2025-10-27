#pragma once

#include <string>
#include <algorithm>
#include <cstdlib>
#include <format>

#include "SDL3/SDL_events.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "tiles/TileManager.hpp"
#include "logging/Logger.hpp"
#include "scene/Scene.hpp"
#include "sdl/SDL_Handler.hpp"
#include "ui/TextHandler.hpp"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include "EditorUI.hpp"

class Editor {
  public:
    Editor();
    ~Editor() = default;

    void run(SDL_State& state);
    void update(SDL_State& state);
    void draw(SDL_State& state);
    void handleInput(SDL_Event& event, SDL_State& state);

  private:
    Tile* previewTile = nullptr;
    TileType lastPreviewType = TILE_TYPE_TERRAIN;
    int lastPreviewIndex = -1;

    int currentMaxIndex = 0;
    bool hasFiniteIndexRange = false;

    void ensurePreviewUpToDate();
    void updatePreviewPosition(SDL_State& state, int tileX, int tileY);
    void drawPreview(SDL_Renderer* renderer);
    void rebuildPreview();
    void clampOrWrapSelectedIndex(int delta);
    int computeMaxIndexFor(TileType type) const;
    const char* tileTypeName(TileType t) const;

    TileType selectedTileType = TILE_TYPE_TERRAIN;
    int selectedTileIndex = 0;

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
    UI::EditorUI ui;
};
