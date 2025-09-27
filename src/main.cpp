#include "sdl/SDL_Handler.hpp"
#include "logging/Logger.hpp"

int main(void) {
  Log::Init();

  const int WINDOW_WIDTH = 1280;
  const int WINDOW_HEIGHT = 720;
  SDL_Handler sdl({
    .title = "PirateEditor",
    .width = WINDOW_WIDTH,
    .height = WINDOW_HEIGHT
  });

  ResourceManager::loadTexture("resources/character/idle/1.png");
  
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