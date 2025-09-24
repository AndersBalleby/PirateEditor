#include "sdl/SDL_Handler.hpp"

int main(void) {
  SDL_Handler sdl(1280, 720, "PirateEditor");

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