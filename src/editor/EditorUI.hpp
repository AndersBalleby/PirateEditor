#pragma once
#include <functional>
#include <string>
#include <SDL3/SDL.h>
#include <algorithm>
#include <format>

#include "SDL3/SDL_rect.h"
#include "ui/TextHandler.hpp"
#include "tiles/TileManager.hpp"
#include "sdl/SDL_Handler.hpp"
#include "logging/Logger.hpp"

namespace UI {
struct EditorUIModel {
  bool        editMode = true;
  bool        showLayers = false;
  int         currentLayer = 0;
  TileType    selectedTileType = TILE_TYPE_TERRAIN;
  int         selectedTileIndex = 0;
  SDL_Texture*selectedTexture = nullptr;
  int         maxIndex = 0;
  int         tileSize = TILE_SIZE;
};

struct EditorUICallbacks {
  std::function<void(int)> setSelectedIndex = nullptr;
  std::function<void(int)> nudgeIndex = nullptr;
  std::function<void()>    toggleLayerView = nullptr;
  std::function<void()>    nextLayer = nullptr;
  std::function<void()>    prevLayer = nullptr;
  std::function<void()>    togglePalette = nullptr;
};

class EditorUI {
public:
  EditorUI() = default;

  void update(SDL_State& state, float dt) noexcept;
  void draw(SDL_State& state, const EditorUIModel& m);
  void handleEvent(const SDL_Event& ev, SDL_State& state,
                     const EditorUIModel& m, const EditorUICallbacks& cb);

  void showSave(const std::string& msg);

  bool paletteVisible() const { return showTilePalette; }
  void setPaletteVisible(bool v) { showTilePalette = v; }

private:
  bool  showSavePopup = false;
  std::string savePopupText;
  float savePopupTimer = 0.0f;
  static constexpr float SAVE_POPUP_DURATION = 2.5f;

  void drawSavePopup(SDL_State& state);

  bool showTilePalette = true;
  SDL_FRect paletteRect { 0.f, 0.f, 480.f, 200.f };
  int   paletteTileSize = 40;
  float paletteMargin   = 8.f;
  float palettePadding  = 4.f;
  int   hoveredTileIndex = -1;

  void drawHUD(SDL_State& state, const EditorUIModel& m);
  void drawTilePalette(SDL_State& state, const EditorUIModel& m);
};

}
