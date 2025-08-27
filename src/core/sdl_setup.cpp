#include "sdl_setup.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_messagebox.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <iostream>
#include <memory>

SDLState initDrivers() {
  SDLState state {};
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fejl: Video Driver",
                                  SDL_GetError(), NULL)) {
      std::cerr
          << "Fejl: Video drivers kunne ikke initialiseres"
          << std::endl; // <-- Fallback hvis ovenstående heller ikke fungere
      return {};
    }
  }

  if(!TTF_Init()) {
    SDL_Log("Kunne ikke indlæse SDL_ttf: %s\n", SDL_GetError());
    return {};
  }
      
  int winWidth = 1280;
  int winHeight = 780;
  SDL_Window *win = SDL_CreateWindow("Game Editor", winWidth, winHeight,
                                     SDL_WINDOW_RESIZABLE);
  if (win == NULL) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fejl: Window Creation",
                             SDL_GetError(), NULL);
    SDL_Quit();
    return {};
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(win, NULL);
  if (renderer == NULL) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fejl: Renderer",
                             SDL_GetError(), NULL);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return {};
  }

  int logW = 1024;
  int logH = 624;
  SDL_SetRenderLogicalPresentation(renderer, logW, logH,
                                   SDL_LOGICAL_PRESENTATION_LETTERBOX);
  SDL_SetDefaultTextureScaleMode(renderer, SDL_SCALEMODE_PIXELART);
  

  state.tRenderer = std::make_unique<TextRenderer>(renderer, "test_assets/ARCADEPI.TTF", 16.0f);
 
  if(!state.tRenderer || !state.tRenderer->isInitialised()) {
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fejl: Text Renderer",
                             SDL_GetError(), NULL);
    SDL_DestroyWindow(win);  
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    return {};
  }
  
  state.window = win;
  state.renderer = renderer;
  state.running = true;
  state.winWidth = winWidth;
  state.winHeight = winHeight;
  state.deltaTime = 0.0f;
    
  return state;
}

// Returnerer false hvis programmet skal stoppe
void handleSDLEvents(SDLState &state) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_EVENT_QUIT:
        state.running = false;
        break;
      case SDL_EVENT_WINDOW_RESIZED:
        state.winWidth = event.window.data1;
        state.winHeight = event.window.data2;
        break;
      }
    }
}
