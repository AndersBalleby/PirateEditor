#pragma once
#include <functional>
#include <string>
#include <SDL3/SDL.h>
#include "SDL3/SDL_rect.h"
#include "ui/TextHandler.hpp"
#include "core/TileManager.hpp"   // for TILE_SIZE + TileType
#include "sdl/SDL_Handler.hpp"    // for SDL_State

namespace UI {

// Data der kommer fra Editor (read-only for UI)
struct EditorUIModel {
    bool        editMode = true;
    bool        showLayers = false;
    int         currentLayer = 0;
    TileType    selectedTileType = TILE_TYPE_TERRAIN;
    int         selectedTileIndex = 0;
    SDL_Texture*selectedTexture = nullptr;    // previewTile->texture
    int         maxIndex = 0;                 // 0-baseret (samme som computeMaxIndexFor)
    int         tileSize = TILE_SIZE;
};

// Callbacks UI kan kalde for at ændre Editor-state
struct EditorUICallbacks {
    std::function<void(int)> setSelectedIndex = nullptr; // set til præcist index
    std::function<void(int)> nudgeIndex = nullptr;       // +1/-1 wrap
    std::function<void()>    toggleLayerView = nullptr;  // TAB
    std::function<void()>    nextLayer = nullptr;        // ↑
    std::function<void()>    prevLayer = nullptr;        // ↓
    std::function<void()>    togglePalette = nullptr;    // P
};

// Let overlay-UI til editoren (HUD, tile palette, save popup)
class EditorUI {
public:
    EditorUI() = default;

    void update(SDL_State& state, float dt) noexcept;
    void draw(SDL_State& state, const EditorUIModel& m);
    void handleEvent(const SDL_Event& ev, SDL_State& state,
                     const EditorUIModel& m, const EditorUICallbacks& cb);

    // kald når der gemmes
    void showSave(const std::string& msg);

    // mulighed for at læse/ændre om paletten vises (hvis du vil styre det udefra)
    bool paletteVisible() const { return showTilePalette; }
    void setPaletteVisible(bool v) { showTilePalette = v; }

private:
    // --- Save popup ---
    bool  showSavePopup = false;
    std::string savePopupText;
    float savePopupTimer = 0.0f;
    static constexpr float SAVE_POPUP_DURATION = 2.5f;

    void drawSavePopup(SDL_State& state);

    // --- Tile palette ---
    bool showTilePalette = true;
    SDL_FRect paletteRect { 0.f, 0.f, 480.f, 200.f };
    int   paletteTileSize = 40;
    float paletteMargin   = 8.f;
    float palettePadding  = 4.f;
    int   hoveredTileIndex = -1;

    void drawHUD(SDL_State& state, const EditorUIModel& m);
    void drawTilePalette(SDL_State& state, const EditorUIModel& m);
};

} // namespace UI
