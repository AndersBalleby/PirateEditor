#include "SDL3/SDL_events.h"
#include "logging/Logger.hpp"
#include "sdl/SDL_Handler.hpp"
#include "core/Editor.hpp"
#include "ui/FPS_Counter.hpp"

int main(void) {
  Log::Init();

  const int WINDOW_WIDTH = 1280;
  const int WINDOW_HEIGHT = 800;
  SDL_Handler sdl({
    .title = "PirateEditor",
    .width = WINDOW_WIDTH,
    .height = WINDOW_HEIGHT
  });

  Editor editor;
  FPS_Counter fpsCounter;

  // Tid målt i millisekunder
  uint64_t lastTime = SDL_GetTicks();
  double deltaTime = 0.0;
  float fps = 0.0f;

  while (sdl.isRunning()) {
    // --- Opdater deltaTime ---
    uint64_t now = SDL_GetTicks();
    deltaTime = (now - lastTime) / 1000.0; // sekunder
    sdl.getState().tickDeltaTime(now, lastTime);
    lastTime = now;

    // --- Event håndtering ---
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT)
        return 0;
      if (event.type == SDL_EVENT_WINDOW_RESIZED) {
        sdl.getState().windowWidth = event.window.data1;
        sdl.getState().windowHeight = event.window.data2;
      }

      // Håndterer input i editor
      editor.handleInput(event);
    }

    // --- Rendering ---
    sdl.clear();

    editor.run(sdl.getState());

    // FPS beregnet ud fra deltaTime
    if (deltaTime > 0.0)
      fps = 1.0f / static_cast<float>(deltaTime);

    fpsCounter.update(sdl.getState());

    sdl.present();

    #if defined(_WIN32) || defined(__linux__)
      sdl.getAudioHandler().playTrack();
    #endif
  }

  return 0;
}
