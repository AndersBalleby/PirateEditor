#include "SDL3/SDL_events.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "core/ResourceManager.hpp"
#include "logging/Logger.hpp"
#include "scene/Background.hpp"
#include "sdl/SDL_Handler.hpp"
#include "core/Editor.hpp"
#include "io/utils.hpp"
#include "math/vec.hpp"
#include "ui/FPS_Counter.hpp"

int main(void) {
  // Sørg for at loggeren er initialiseret
  Log::Init();

  // Opret vindue
  const int WINDOW_WIDTH = 1280;
  const int WINDOW_HEIGHT = 800;
  SDL_Handler sdl({
    .title = "PirateEditor",
    .width = WINDOW_WIDTH,
    .height = WINDOW_HEIGHT
  });

  // Editor
  Editor editor;

  // FPS Counter
  FPS_Counter fpsCounter = FPS_Counter();

  // DeltaTime
  uint64_t now = SDL_GetTicks();
  uint64_t last = 0;
  double deltaTime = 0.0;

  float mouseX, mouseY;
  float mapHeight = 704.0f;
  float mapOffsetY = 0.0f;
  while(sdl.isRunning()) {
    // Opdater deltaTime
    last = now;
    now = SDL_GetTicks();
    sdl.getState().tickDeltaTime(now, last);

    // Håndter events
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_EVENT_QUIT) return 0;
      if(event.type == SDL_EVENT_WINDOW_RESIZED) {
        sdl.getState().windowWidth = event.window.data1;
        sdl.getState().windowHeight = event.window.data2;
      }
    }

    // Gør rendering klar
    sdl.clear();

    SDL_GetMouseState(&mouseX, &mouseY);

    float worldMouseX = mouseX + sdl.getState().cameraPos.x;
    float worldMouseY = mouseY - mapOffsetY;

    int tileX = (int) std::floor(worldMouseX / TILE_SIZE);
    int tileY = (int) std::floor(worldMouseY / TILE_SIZE);

    // Kør et editor loop
    editor.run(sdl.getState());

    mapOffsetY = sdl.getState().windowHeight - mapHeight;
    if(mapOffsetY < 0) mapOffsetY = 0; // hvis vinduet er mindre end map

    sdl.getState().cameraPos.x += sdl.getState().cameraX;

    SDL_SetRenderDrawColor(sdl.getRenderer(), 120, 135, 150, 255);

    // Kan maks gå -64 * 8 og 64 * 47 på x-aksen. TODO: Få lavet en ordentlig camera class
    for(int i = (-64 * 8); i <= sdl.getState().windowWidth + (64 * 47); i += TILE_SIZE) {
      SDL_RenderLine(sdl.getRenderer(), i - sdl.getState().cameraPos.x, TILE_SIZE + mapOffsetY, i - sdl.getState().cameraPos.x, sdl.getState().windowHeight);
    }

    for(int j = 64; j <= sdl.getState().windowHeight; j += TILE_SIZE) {
      SDL_RenderLine(sdl.getRenderer(), 0, j + mapOffsetY, sdl.getState().windowWidth, j + mapOffsetY);
    }

    // Der er nogle alignment issues her med kameraet ift. markeringen, men er ikke sikker på om jeg kan fikse det.
    if(worldMouseY >= 64.0f) {
      SDL_SetRenderDrawColor(sdl.getRenderer(), 30, 150, 255, 255);
      SDL_FRect rect = {roundf(tileX * TILE_SIZE - sdl.getState().cameraPos.x), tileY * TILE_SIZE + mapOffsetY, TILE_SIZE + 1.0f, TILE_SIZE + 1.0f};
      SDL_RenderRect(sdl.getRenderer(), &rect);
    }

    // fpsCounter.update håndterer både update og draw
    fpsCounter.update(sdl.getState());

    sdl.present();
  }

  return 0;
}
