#include "Editor.hpp"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include "core/TileManager.hpp"
#include "logging/Logger.hpp"
#include <algorithm>
#include <cstdlib>

Editor::Editor()
  : scene_manager(0, "TestScene")
{
  Log::Info("Initialiserer editor");
}

void Editor::drawGridLines(SDL_State& state) {
  auto mouseState = SDL_GetMouseState(&mouseX, &mouseY);

  mapOffsetY = std::max(0.0f, state.windowHeight - mapHeight);
  state.cameraPos.x += state.cameraX;

  const float worldMouseX = mouseX + state.cameraPos.x;
  const float worldMouseY = mouseY - mapOffsetY;

  const int tileX = static_cast<int>(std::floor(worldMouseX / TILE_SIZE));
  const int tileY = static_cast<int>(std::floor(worldMouseY / TILE_SIZE));

  SDL_SetRenderDrawColor(state.renderer, gridColor.r, gridColor.g, gridColor.b, gridColor.a);

  // TODO: Flyt magiske tal ud i en Camera-klasse senere
  const int startX = -64 * 8;
  const int endX = state.windowWidth + 64 * 47;
  if(!editMode) return;

  for (int x = startX; x <= endX; x += TILE_SIZE) {
    SDL_RenderLine(state.renderer, x - state.cameraPos.x, TILE_SIZE + mapOffsetY, x - state.cameraPos.x, state.windowHeight);
  }

  for (int y = 64; y <= state.windowHeight; y += TILE_SIZE) {
    SDL_RenderLine(state.renderer, 0, y + mapOffsetY, state.windowWidth, y + mapOffsetY);
  }

  if (worldMouseY >= 64.0f) {
    SDL_SetRenderDrawColor(state.renderer, hoverColor.r, hoverColor.g, hoverColor.b, hoverColor.a);
    SDL_FRect rect{
      std::roundf(tileX * TILE_SIZE - state.cameraPos.x),
      tileY * TILE_SIZE + mapOffsetY,
      TILE_SIZE + 1.0f,
      TILE_SIZE + 1.0f
    };
    SDL_RenderRect(state.renderer, &rect);


    bool leftClick = mouseState & SDL_BUTTON_LMASK;
    if(leftClick && !wasMouseDown) {
      if(showLayers) {
        Tile* existingTile = scene_manager.getTileAt(tileX, tileY);
        if(!existingTile || existingTile->getType() != selectedTileType) {
          Tile* newTile = TileFactory::createTile(
              selectedTileType,
              { float(tileX), float(tileY) },
              selectedTileIndex
          );
          scene_manager.addTileToLayer(newTile, currentLayer);
        }
      } else {
        Tile* existingTile = scene_manager.getTileAt(tileX, tileY);
        if(!existingTile || existingTile->getType() != selectedTileType) {
          Tile* newTile = TileFactory::createTile(
              selectedTileType,
              { float(tileX), float(tileY) },
              selectedTileIndex
            );
          scene_manager.addTileToLayer(newTile, currentLayer);
        }
      }
    }

    if (state.keyState[SDL_SCANCODE_DELETE] ||
        state.keyState[SDL_SCANCODE_BACKSPACE] ||
        (mouseState & SDL_BUTTON_RMASK)) {
      for (auto [tx, ty] : selectedTiles) {
        if (showLayers) {
          // Slet kun currentLayer
          scene_manager.removeLayerTiles(tx, ty, currentLayer);
        } else {
          // Slet alle layers
          for (int layer = 0; layer < maxLayers; ++layer) {
            scene_manager.removeLayerTiles(tx, ty, layer);
          }
        }
      }

      int tileX = static_cast<int>(std::floor((mouseX + state.cameraPos.x) / TILE_SIZE));
      int tileY = static_cast<int>(std::floor((mouseY - mapOffsetY) / TILE_SIZE));

      if (showLayers) {
          scene_manager.removeTileAt(tileX, tileY, currentLayer);
      } else {
          for (int layer = 0; layer < maxLayers; ++layer) {
              scene_manager.removeTileAt(tileX, tileY, layer);
          }
      }

      selectedTiles.clear();
    }

    // --- PREVIEW TILE ---
    ensurePreviewUpToDate();
    if (previewTile) {
      if (lastPreviewIndex != selectedTileIndex) {
        previewTile->setTileIndex(selectedTileIndex);
        lastPreviewIndex = selectedTileIndex;
      }
      updatePreviewPosition(state, tileX, tileY);
      drawPreview(state.renderer);
    }

    }

    bool leftDown = mouseState & SDL_BUTTON_LMASK & state.keyState[SDL_SCANCODE_LCTRL];
    if(leftDown && !wasMouseDown) {
      isSelecting = true;
      selectionStart = {mouseX, mouseY};
      selectionEnd = selectionStart;
    }

    if(isSelecting && leftDown) {
      selectionEnd = {mouseX, mouseY};
    }

    if(!leftDown && wasMouseDown && isSelecting) {
      isSelecting = false;
      updateSelectedTiles(state);
    }

    wasMouseDown = leftDown;

    if (isSelecting) {
      float x = std::min(selectionStart.x, selectionEnd.x);
      float y = std::min(selectionStart.y, selectionEnd.y);
      float w = std::abs(selectionEnd.x - selectionStart.x);
      float h = std::abs(selectionEnd.y - selectionStart.y);

      SDL_SetRenderDrawColor(state.renderer, 0, 0, 255, 60);
      SDL_FRect rect = { x, y, w, h };
      SDL_RenderRect(state.renderer, &rect);
    }

    // Tegn de færdigt markerede tiles
    for (auto [tx, ty] : selectedTiles) {
      SDL_FRect rect{
        tx * TILE_SIZE - state.cameraPos.x,
        ty * TILE_SIZE + mapOffsetY,
        TILE_SIZE + 1.0f,
        TILE_SIZE + 1.0f
      };
      SDL_SetRenderDrawBlendMode(state.renderer, SDL_BLENDMODE_BLEND);

      SDL_SetRenderDrawColor(state.renderer, gridColor.r, gridColor.g, gridColor.b, 100);
      SDL_RenderFillRect(state.renderer, &rect);
    }
}

const char* Editor::tileTypeName(TileType t) const {
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
}

int Editor::computeMaxIndexFor(TileType type) const {
  switch (type) {
    case TILE_TYPE_TERRAIN:
    case TILE_TYPE_GRASS:
    case TILE_TYPE_PLAYER_SETUP:
    case TILE_TYPE_ENEMY:
    case TILE_TYPE_COIN:
    case TILE_TYPE_CONSTRAINT: {
      // Vi bruger previewTile->texture til at beregne antal ruder i sheets.
      if (!previewTile || !previewTile->texture) return 0;
      float w = 0, h = 0;
      SDL_GetTextureSize(previewTile->texture, &w, &h);
      int cols = int(w) / TILE_SIZE;
      int rows = int(h) / TILE_SIZE;
      int total = std::max(1, cols * rows);
      return total - 1; // 0-baseret
    }
    case TILE_TYPE_FG_PALM:
      return 1;
    case TILE_TYPE_BG_PALM:
    case TILE_TYPE_CRATE:
      return 0;
    default:
      return 0;
  }
}

void Editor::rebuildPreview() {
  // Ryd hvis eksisterer
  if (previewTile) {
    delete previewTile;
    previewTile = nullptr;
  }
  // Byg preview tile ved (0,0) – position opdateres løbende
  previewTile = TileFactory::createTile(selectedTileType, {0.f, 0.f}, selectedTileIndex);
  lastPreviewType = selectedTileType;
  lastPreviewIndex = selectedTileIndex;

  // Opdater max index info
  currentMaxIndex = computeMaxIndexFor(selectedTileType);

  // Marker om vi har et “begrænset” indexområde (til wrap)
  hasFiniteIndexRange = true;

  // Special-case FG_PALM: brug {1,2}
  if (selectedTileType == TILE_TYPE_FG_PALM) {
    if (selectedTileIndex != 1 && selectedTileIndex != 2) {
      // normaliser til gyldigt sæt
      selectedTileIndex = 1;
      if (previewTile) previewTile->setTileIndex(selectedTileIndex);
      lastPreviewIndex = selectedTileIndex;
    }
  }
}

void Editor::ensurePreviewUpToDate() {
  if (!previewTile || lastPreviewType != selectedTileType || lastPreviewIndex != selectedTileIndex) {
    rebuildPreview();
  }
}

void Editor::updatePreviewPosition(SDL_State& state, int tileX, int tileY) {
  if (!previewTile) return;

  // Sæt grid-position, lav ‘update’ (så dstRect.x/y beregnes) og læg mapOffsetY på y
  previewTile->position = { float(tileX), float(tileY) };
  previewTile->update({ state.cameraPos.x, 0.0f });
  previewTile->dstRect.y += mapOffsetY;
}

void Editor::drawPreview(SDL_Renderer* renderer) {
  if (!previewTile || !previewTile->texture) return;

  Uint8 oldAlpha = 255;
  SDL_GetTextureAlphaMod(previewTile->texture, &oldAlpha);
  SDL_SetTextureAlphaMod(previewTile->texture, 150); // ghost-look
  previewTile->draw(renderer);
  SDL_SetTextureAlphaMod(previewTile->texture, oldAlpha);
}

void Editor::clampOrWrapSelectedIndex(int delta) {
  // FG_PALM special: {1,2}
  if (selectedTileType == TILE_TYPE_FG_PALM) {
    if (delta > 0) selectedTileIndex = (selectedTileIndex == 1 ? 2 : 1);
    else           selectedTileIndex = (selectedTileIndex == 2 ? 1 : 2);
    return;
  }

  // “normale” tilemaps eller statics (statics har max=0)
  currentMaxIndex = computeMaxIndexFor(selectedTileType);
  int next = selectedTileIndex + delta;

  if (currentMaxIndex <= 0) {
    selectedTileIndex = 0;
    return;
  }

  // wrap 0..currentMaxIndex
  if (next < 0) next = currentMaxIndex;
  if (next > currentMaxIndex) next = 0;

  selectedTileIndex = next;
}


void Editor::handleInput(SDL_Event &event) {
  if(event.type == SDL_EVENT_KEY_DOWN) {
    if(event.key.key == SDLK_SPACE) {
      editMode = !editMode;
    }

    if(event.key.key == SDLK_TAB) {
      showLayers = !showLayers;
    }

    if(showLayers) {
      if(event.key.key == SDLK_UP) {
        currentLayer = (currentLayer + 1) % maxLayers;
      }
      if(event.key.key == SDLK_DOWN) {
        currentLayer = (currentLayer - 1 + maxLayers) % maxLayers;
      }
    }

    if(editMode) {
      if(event.key.key == SDLK_1) { selectedTileType = TILE_TYPE_TERRAIN; }
      if(event.key.key == SDLK_2) { selectedTileType = TILE_TYPE_CRATE; }
      if(event.key.key == SDLK_3) { selectedTileType = TILE_TYPE_GRASS; }
      if(event.key.key == SDLK_4) { selectedTileType = TILE_TYPE_PLAYER_SETUP; }
      if(event.key.key == SDLK_5) { selectedTileType = TILE_TYPE_ENEMY; }
      if(event.key.key == SDLK_6) { selectedTileType = TILE_TYPE_COIN; }
      if(event.key.key == SDLK_7) { selectedTileType = TILE_TYPE_FG_PALM; }
      if(event.key.key == SDLK_8) { selectedTileType = TILE_TYPE_BG_PALM; }
      if(event.key.key == SDLK_9) { selectedTileType = TILE_TYPE_CONSTRAINT; }
    }
  }
  if (event.type == SDL_EVENT_MOUSE_WHEEL) {
      // event.wheel.y > 0 = scroll op, < 0 = scroll ned
      if (event.wheel.y > 0) {
          clampOrWrapSelectedIndex(+1);
      } else if (event.wheel.y < 0) {
          clampOrWrapSelectedIndex(-1);
      }
  }

}


void Editor::updateSelectedTiles(SDL_State& state) {
  selectedTiles.clear();

  float x = std::min(selectionStart.x, selectionEnd.x) + state.cameraPos.x;
  float y = std::min(selectionStart.y, selectionEnd.y) - mapOffsetY;
  float w = std::abs(selectionEnd.x - selectionStart.x);
  float h = std::abs(selectionEnd.y - selectionStart.y);

  int startTileX = (int) std::floor(x / TILE_SIZE);
  int startTileY = (int) std::floor(y / TILE_SIZE);
  int endTileX   = (int) std::floor((x + w) / TILE_SIZE);
  int endTileY   = (int) std::floor((y + h) / TILE_SIZE);

  for(int ty = startTileY; ty <= endTileY; ++ty) {
    if((ty * TILE_SIZE + mapOffsetY) < 64.0f + mapOffsetY) continue;

    for(int tx = startTileX; tx <= endTileX; ++tx) {
      selectedTiles.emplace_back(tx, ty);
    }
  }
}

void Editor::update(SDL_State& state) {
  scene_manager.update(state);
}

const std::string layers[] = {"{green}Background", "{green}Terrain", "{green}Foreground"};
void Editor::draw(SDL_State& state) {
  if (showLayers) {
    // Tegn alt transparent undtagen currentLayer
    scene_manager.draw(state.renderer, currentLayer);
  } else {
    // Normal visning – tegn alt
    scene_manager.draw(state.renderer, -1);
  }

  drawGridLines(state);

  std::string editText = std::format("Edit mode: {}", editMode ? "{green}ON" : "{red}OFF");
  UI::Text::displayText(editText, {10.0f, 30.0f});

  std::string typeText = std::format("Selected type: {}{}", "{green}", tileTypeName(selectedTileType));
  UI::Text::displayText(typeText, {10.0f, 50.0f});

  currentMaxIndex = computeMaxIndexFor(selectedTileType);
  int shownMax = (selectedTileType == TILE_TYPE_FG_PALM) ? 2 : (currentMaxIndex + 1); // vis som 1-baseret count
  int shownIdx = selectedTileIndex;

  if (selectedTileType != TILE_TYPE_FG_PALM) {
    shownIdx = selectedTileIndex + 1;
  }

  std::string idxText = std::format("Tile index: {0}/{1}", "{green}" + std::to_string(shownIdx) + "{white}", "{green}" + std::to_string(shownMax));
  UI::Text::displayText(idxText, {10.0f, 70.0f});

  std::string layerViewText = std::format("Layer View (TAB): {}", showLayers ? "{green}ON" : "{red}OFF");
  UI::Text::displayText(layerViewText, {10.0f, 90.0f});


  if(showLayers) {
    std::string layerText = std::format("Layer: {}", layers[currentLayer]);
    UI::Text::displayText(layerText, {10.0f, 110.0f});
  }
}

void Editor::run(SDL_State& state) {
  this->update(state);
  this->draw(state);
}
