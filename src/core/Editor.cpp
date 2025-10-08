#include "Editor.hpp"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_scancode.h"

Editor::Editor()
  : scene_manager(0, "TestScene")
{
  Log::Info("Initialiserer editor");
}


void Editor::drawGridLines(SDL_State& state) {
    SDL_GetMouseState(&mouseX, &mouseY);

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

        if(state.keyState[SDL_SCANCODE_DELETE] || state.keyState[SDL_SCANCODE_BACKSPACE]) {
          scene_manager.removeTileAt(tileX, tileY);
        }
    }
}

void Editor::update(SDL_State& state) {
  scene_manager.update(state);
}

void Editor::draw(SDL_State& state) {
  scene_manager.draw(state.renderer);
  drawGridLines(state);
}

void Editor::run(SDL_State& state) {
  this->update(state);
  this->draw(state);
}
