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
      if(event.key.key == SDLK_1) selectedTileType = TILE_TYPE_TERRAIN;
      if(event.key.key == SDLK_2) selectedTileType = TILE_TYPE_CRATE;
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
  std::string layerViewText = std::format("Layer View (TAB): {}", showLayers ? "{green}ON" : "{red}OFF");
  UI::Text::displayText(editText, {10.0f, 30.0f});
  UI::Text::displayText(layerViewText, {10.0f, 50.0f});

  if(showLayers) {
    std::string layerText = std::format("Layer: {}", layers[currentLayer]);
    UI::Text::displayText(layerText, {10.0f, 70.0f});
  }
}

void Editor::run(SDL_State& state) {
  this->update(state);
  this->draw(state);
}
