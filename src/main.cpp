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
#include <SDL3_ttf/SDL_ttf.h>
#include <cmath>
#include <cstdio>

#include "core/SDLHandler.hpp"
#include "editor/editor.hpp"
#include "editor/ui.hpp"

int main(void) {
  SDLHandler sdl; 
  // Skal editoren køre?
  const bool editorRun = true;
  Editor editor = Editor(sdl.getRenderer());
  UI ui = UI(sdl.getRenderer(), sdl.getTextRenderer());
  
  while (sdl.isRunning()) {
    sdl.tickDeltaTime();

    // SDL Events (Input)
    sdl.handleEvents();
    
    // Start rendering
    SDL_SetRenderDrawColor(sdl.getRenderer(), 20, 10, 30, 255);
    SDL_RenderClear(sdl.getRenderer());

    // Editor
    if (editorRun) {
      editor.run(sdl.getRenderer(), ui, sdl.getDeltaTime());
    }

    // Present to screen
    SDL_RenderPresent(sdl.getRenderer());
  }

  return 0;
}
