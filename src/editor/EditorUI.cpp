#include "EditorUI.hpp"

namespace UI {

void EditorUI::update(SDL_State& state, float dt) noexcept {
  if (showSavePopup) {
    savePopupTimer -= dt;
    if (savePopupTimer <= 0.f) {
      showSavePopup = false;
    }
  }
}

void EditorUI::draw(SDL_State& state, const EditorUIModel& m) {
  drawHUD(state, m);
  if (showTilePalette) drawTilePalette(state, m);
  if (showSavePopup)   drawSavePopup(state);
}

void EditorUI::handleEvent(const SDL_Event& ev, SDL_State& state, const EditorUIModel& m, const EditorUICallbacks& cb) {
  if (ev.type == SDL_EVENT_KEY_DOWN) {
    if (ev.key.key == SDLK_TAB && cb.toggleLayerView) cb.toggleLayerView();
    if (ev.key.key == SDLK_P   && cb.togglePalette)   cb.togglePalette();
    if (m.showLayers) {
      if (ev.key.key == SDLK_UP   && cb.nextLayer) cb.nextLayer();
      if (ev.key.key == SDLK_DOWN && cb.prevLayer) cb.prevLayer();
    }
  }

  // Mouse wheel over paletten -> skift index
  if (ev.type == SDL_EVENT_MOUSE_WHEEL && showTilePalette) {
    float mx, my;
    SDL_GetMouseState(&mx, &my);
    // paletten ligger i screen-space øverst højre hjørne
    if (mx >= paletteRect.x && mx <= (paletteRect.x + paletteRect.w) &&
        my >= paletteRect.y && my <= (paletteRect.y + paletteRect.h)) {
          if (cb.nudgeIndex) {
            if (ev.wheel.y > 0) cb.nudgeIndex(+1);
            else if (ev.wheel.y < 0) cb.nudgeIndex(-1);
          }
        }
    }

}

void EditorUI::showSave(const std::string& msg) {
  savePopupText = msg;
  savePopupTimer = SAVE_POPUP_DURATION;
  showSavePopup = true;
}

void EditorUI::drawHUD(SDL_State& state, const EditorUIModel& m) {
  using UI::Text;

  std::string editText  = std::format("Edit mode: {}", m.editMode ? "{green}ON" : "{red}OFF");
  Text::displayText(editText, {10.f, 30.f});

  auto tileTypeName = [&](TileType t)->const char*{
    switch (t) {
      case TILE_TYPE_TERRAIN:        return "Terrain";
      case TILE_TYPE_CRATE:          return "Crate";
      case TILE_TYPE_GRASS:          return "Grass";
      case TILE_TYPE_PLAYER_SETUP:   return "PlayerSetup";
      case TILE_TYPE_ENEMY:          return "Enemy";
      case TILE_TYPE_COIN:           return "Coin";
      case TILE_TYPE_FG_PALM:        return "FG Palm";
      case TILE_TYPE_BG_PALM:        return "BG Palm";
      case TILE_TYPE_FG_PALM_LARGE:  return "FG Palm Large";
      case TILE_TYPE_FG_PALM_SMALL:  return "FG Palm Small";
      case TILE_TYPE_CONSTRAINT:     return "Constraint";
      default:                       return "Unknown";
    }
  };

  std::string typeText = std::format("Selected type: {}{}", "{green}", tileTypeName(m.selectedTileType));
  Text::displayText(typeText, {10.f, 50.f});

  int shownMax = (m.selectedTileType == TILE_TYPE_FG_PALM) ? 2 : (m.maxIndex + 1);
  int shownIdx = (m.selectedTileType == TILE_TYPE_FG_PALM) ? m.selectedTileIndex : (m.selectedTileIndex + 1);
  std::string idxText = std::format("Tile index: {0}/{1}",
      "{green}"+std::to_string(shownIdx)+"{white}",
      "{green}"+std::to_string(shownMax));
  Text::displayText(idxText, {10.f, 70.f});

  std::string layerViewText = std::format("Layer View (TAB): {}", m.showLayers ? "{green}ON" : "{red}OFF");
  Text::displayText(layerViewText, {10.f, 90.f});

  if (m.showLayers) {
    static const std::string layers[] = {"{green}Background", "{green}Terrain", "{green}Foreground"};
    std::string layerText = std::format("Layer: {}", layers[m.currentLayer]);
    Text::displayText(layerText, {10.f, 110.f});
  }
}

void EditorUI::drawTilePalette(SDL_State& state, const EditorUIModel& m) {
  if (!m.selectedTexture) return;

  // placer i øverste højre
  paletteRect.x = state.windowWidth - paletteRect.w - 10.f;
  paletteRect.y = 10.f;

  // baggrund
  SDL_SetRenderDrawColor(state.renderer, 20, 20, 30, 180);
  SDL_RenderFillRect(state.renderer, &paletteRect);
  SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 100);
  SDL_RenderRect(state.renderer, &paletteRect);

  // label
  UI::Text::displayText(std::format("Toggle Palette: {}P", "{green}"), {paletteRect.x + 250.f, paletteRect.y + 15.f});

  float texW = 0, texH = 0;
  SDL_GetTextureSize(m.selectedTexture, &texW, &texH);

  const int cols = int(texW) / m.tileSize;
  const int rows = int(texH) / m.tileSize;
  const float startX = paletteRect.x + paletteMargin;
  const float startY = paletteRect.y + paletteMargin;

  auto isTilemapType = (m.selectedTileType == TILE_TYPE_TERRAIN ||
                        m.selectedTileType == TILE_TYPE_GRASS ||
                        m.selectedTileType == TILE_TYPE_PLAYER_SETUP ||
                        m.selectedTileType == TILE_TYPE_ENEMY ||
                        m.selectedTileType == TILE_TYPE_COIN ||
                        m.selectedTileType == TILE_TYPE_CONSTRAINT);

  if (isTilemapType && cols > 0 && rows > 0) {
    int maxIndex = m.maxIndex;
    hoveredTileIndex = -1;

    // highlight ved hover
    float mx, my;
    SDL_GetMouseState(&mx, &my);

    for (int ty = 0; ty < rows; ++ty) {
        for (int tx = 0; tx < cols; ++tx) {
            int idx = ty * cols + tx;
            if (idx > maxIndex) break;

            SDL_FRect dst{
                startX + tx * (paletteTileSize + palettePadding),
                startY + ty * (paletteTileSize + palettePadding),
                (float)paletteTileSize,
                (float)paletteTileSize
            };
            SDL_FRect src{
                (float)(tx * m.tileSize),
                (float)(ty * m.tileSize),
                (float)m.tileSize,
                (float)m.tileSize
            };

            SDL_RenderTexture(state.renderer, m.selectedTexture, &src, &dst);

            // mark selected
            if (idx == m.selectedTileIndex) {
                SDL_SetRenderDrawColor(state.renderer, 255, 220, 0, 90);
                SDL_RenderFillRect(state.renderer, &dst);
                SDL_SetRenderDrawColor(state.renderer, 255, 220, 0, 200);
                SDL_RenderRect(state.renderer, &dst);
            }

            if (mx >= dst.x && mx <= dst.x + dst.w &&
                my >= dst.y && my <= dst.y + dst.h) {
                hoveredTileIndex = idx;
                SDL_SetRenderDrawColor(state.renderer, 0, 180, 255, 120);
                SDL_RenderRect(state.renderer, &dst);
            }
        }
    }
    return;
  }

  // (crate/palms/constraint)
  const float previewSize_palm  = 80.f;
  const float previewSize_crate = 58.f;
  const float baseX = startX + 10.f;
  const float baseY = startY + 10.f;

  SDL_FRect dstRect_palm  { baseX, baseY, previewSize_palm,  previewSize_palm };
  SDL_FRect dstRect_crate { baseX, baseY, previewSize_crate, previewSize_crate };
  SDL_FRect srcRect { 0, 0, texW, texH };

  if (m.selectedTileType == TILE_TYPE_CRATE || m.selectedTileType == TILE_TYPE_BG_PALM || m.selectedTileType == TILE_TYPE_CONSTRAINT) {
    if (m.selectedTileType == TILE_TYPE_CRATE) {
      SDL_RenderTexture(state.renderer, m.selectedTexture, &srcRect, &dstRect_crate);
      SDL_SetRenderDrawColor(state.renderer, 255, 220, 0, 120);
      SDL_RenderRect(state.renderer, &dstRect_crate);
    } else {
      SDL_RenderTexture(state.renderer, m.selectedTexture, &srcRect, &dstRect_palm);
      SDL_SetRenderDrawColor(state.renderer, 255, 220, 0, 120);
      SDL_RenderRect(state.renderer, &dstRect_palm);
    }
    return;
  }

  if (m.selectedTileType == TILE_TYPE_FG_PALM) {
    SDL_Texture* small = ResourceManager::loadTexture("resources/terrain/palm_small/small_1.png");
    SDL_Texture* large = ResourceManager::loadTexture("resources/terrain/palm_large/large_1.png");

    float smallW = 0, smallH = 0, largeW = 0, largeH = 0;
    SDL_GetTextureSize(small, &smallW, &smallH);
    SDL_GetTextureSize(large, &largeW, &largeH);

    SDL_FRect smallDst { baseX,          baseY,      64, 64 };
    SDL_FRect largeDst { baseX + 100.f,  baseY - 10, 64, 96 };

    SDL_RenderTexture(state.renderer, small, nullptr, &smallDst);
    SDL_RenderTexture(state.renderer, large, nullptr, &largeDst);

    SDL_SetRenderDrawColor(state.renderer, 255, 220, 0, 200);
    if (m.selectedTileIndex == 1) SDL_RenderRect(state.renderer, &smallDst);
    else                          SDL_RenderRect(state.renderer, &largeDst);
    return;
  }
}

void EditorUI::drawSavePopup(SDL_State& state) {
  const float popupW = 400.f;
  const float popupH = 60.f;
  const float popupX = (state.windowWidth - popupW) / 2.f;
  const float popupY = state.windowHeight - popupH - 40.f;

  SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);
  SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 180);
  SDL_FRect rect { popupX, popupY, popupW, popupH };
  SDL_RenderFillRect(state.renderer, &rect);

  SDL_SetRenderDrawColor(state.renderer, 255, 255, 255, 180);
  SDL_RenderRect(state.renderer, &rect);

  UI::Text::displayText(savePopupText, { popupX + 20.f, popupY + 20.f });
}

}
