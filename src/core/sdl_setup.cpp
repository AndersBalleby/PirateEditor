#include "sdl_setup.hpp"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_messagebox.h>
#include <iostream>

SDLState initDrivers() {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    if (!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fejl: Video Driver",
                                  SDL_GetError(), NULL)) {
      std::cerr
          << "Fejl: Video drivers kunne ikke initialiseres"
          << std::endl; // <-- Fallback hvis ovenstående heller ikke fungere
      return {};
    }
  }

  int winWidth = 1280;
  int winHeight = 780;
  SDL_Window *win = SDL_CreateWindow("Game Engine", winWidth, winHeight,
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
  
  return {
    .window = win,
    .renderer = renderer,
    .running = true,
    .winWidth = winWidth,
    .winHeight = winHeight,
    .deltaTime = 0.0f,
  };
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
