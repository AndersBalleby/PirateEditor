#include "sdl/SDL_Handler.hpp"
#include "core/Editor.hpp"
#include "io/utils.hpp"


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

  Utils::Layout test = Utils::LoadCSVFile("levels/0/level_0_bg_palms.csv");

  while(sdl.isRunning()) {
    
    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_EVENT_QUIT) return 0;
    }

    sdl.clear();

    sdl.present();
  }

  return 0;
}