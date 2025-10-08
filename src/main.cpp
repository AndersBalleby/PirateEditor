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

    // Kør et editor loop
    editor.run(sdl.getState());

    // fpsCounter.update håndterer både update og draw
    fpsCounter.update(sdl.getState());

    sdl.present();
  }

  return 0;
}
