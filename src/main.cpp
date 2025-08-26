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

#include "core/TextRenderer.hpp"
#include "core/sdl_setup.hpp"
#include "editor/editor.hpp"
#include "editor/ui.hpp"

int main(void) {
  SDLState state = initDrivers();
  if(state.window == NULL || state.renderer == NULL) {
    return 1;
  }
   
  // Skal editoren køre?
  const bool editorRun = true;
  Editor editor = Editor(state.renderer);
  UI ui = UI();
  
  // FPS Tekst
  char buffer[128];
  std::snprintf(buffer, sizeof(buffer), "FPS: 0");
  SDL_Color color = { 255, 255, 255, SDL_ALPHA_OPAQUE };
  Text fps_text(state.renderer, state.tRenderer->getFont(), buffer, 1.0f, color);
  
  // FPS counter
  uint64_t prevCounter = SDL_GetPerformanceCounter();
  uint64_t freq = SDL_GetPerformanceFrequency();
  double fpsDeltaTimer = 0.0;
  int fps = 0;

  while (state.running) {
    uint64_t nowCounter = SDL_GetPerformanceCounter();
    state.deltaTime = (double)(nowCounter - prevCounter) / (double) freq;
    prevCounter = nowCounter;
    
    fpsDeltaTimer += state.deltaTime;
    
    // update
    handleSDLEvents(state);
    
    SDL_SetRenderDrawColor(state.renderer, 20, 10, 30, 255);
    SDL_RenderClear(state.renderer);

    // Editor
    if(editorRun) {
      editor.run(state.renderer, ui, state.deltaTime);
    }

    if(fpsDeltaTimer >= 1.0) {
      fps = (state.deltaTime > 0.0) ? (int)(1.0 / state.deltaTime) : 0;
      std::snprintf(buffer, sizeof(buffer), "FPS: %d", fps);
      fps_text.updateMessage(state.renderer, state.tRenderer->getFont(), buffer, color);
      fpsDeltaTimer = 0.0;
    }

    state.tRenderer->displayText(fps_text, 1024-125, 10);

    SDL_RenderPresent(state.renderer);
    
  }
  
  SDL_DestroyRenderer(state.renderer);
  SDL_DestroyWindow(state.window);
  SDL_Quit();

  return 0;
}


