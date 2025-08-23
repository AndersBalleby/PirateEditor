#include <SDL3/SDL.h>
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <SDL3_image/SDL_image.h>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <ostream>

#include "core/sdl_setup.hpp"
#include "game/animation.hpp"


int main(void) {
  SDLState state = initDrivers();
  if(state.window == NULL || state.renderer == NULL) {
    return 1;
  }
 
  Animation test("test", state.renderer, "test_assets/01-Idle");

  SDL_FRect src = {0, 0, 64, 40};
  SDL_FRect dest = {0, 0, 64 * 4, 40 * 4};
  
  uint64_t prevTime = SDL_GetTicks();
  SDL_Texture *sky = IMG_LoadTexture(state.renderer, "test_assets/sky.png");
  SDL_FRect sky_src = {0, 0, static_cast<float>(sky->w), static_cast<float>(sky->h)};
  SDL_FRect sky_dest = {0, 0, static_cast<float>(state.winWidth), static_cast<float>(state.winWidth)};
  if(!sky) {
    std::cerr << SDL_GetError() << std::endl;
  }
  while (state.running) {
    uint64_t nowTime = SDL_GetTicks();
    state.deltaTime = (nowTime - prevTime) / 1000.0f;

    // update
    handleSDLEvents(state); 
    test.tick(state.deltaTime);
    
    SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
    SDL_RenderClear(state.renderer);

    // Render alle textures her
    SDL_RenderTexture(state.renderer, sky, &sky_src, &sky_dest);
    SDL_RenderTexture(state.renderer, test.getCurrentTexture(), &src, &dest);
    SDL_RenderPresent(state.renderer);
    
    // Opdater til deltaTime
    prevTime = nowTime;
  }

  SDL_DestroyTexture(sky);
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();

  return 0;
}


