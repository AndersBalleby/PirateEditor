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
    deltaTime = (now - last) / 1000.0;

    // Håndter events
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_EVENT_QUIT) return 0;
    }

    // Gør rendering klar
    sdl.clear();

    // Kør et editor loop
    editor.run(sdl.getRenderer(), deltaTime);

    // fpsCounter.update håndterer både update og draw
    fpsCounter.update(sdl.getRenderer(), sdl.getFont(), deltaTime);
    sdl.present();
  }

  return 0;
}
