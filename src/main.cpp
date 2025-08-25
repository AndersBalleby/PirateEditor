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

#include "core/sdl_setup.hpp"
#include "editor/editor.hpp"
#include "editor/ui.hpp"

int main(void) {
  SDLState state = initDrivers();
  if(state.window == NULL || state.renderer == NULL) {
    return 1;
  }
  
  uint64_t prevTime = SDL_GetTicks();
  
  // Skal editoren køre?
  const bool editorRun = true;
  Editor editor = Editor(state.renderer);
  UI ui = UI();

  while (state.running) {
    uint64_t nowTime = SDL_GetTicks();
    state.deltaTime = (nowTime - prevTime) / 1000.0f;

    // update
    handleSDLEvents(state);
    
    SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
    SDL_RenderClear(state.renderer);
 
    // Editor
    if(editorRun) {
      editor.run(state.renderer, ui, state.deltaTime);
    }

    SDL_RenderPresent(state.renderer);
    
    // Opdater til deltaTime
    prevTime = nowTime;
  }

  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();

  return 0;
}


