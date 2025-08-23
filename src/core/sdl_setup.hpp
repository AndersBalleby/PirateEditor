#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>

struct SDLState {
  SDL_Window *window;
  SDL_Renderer *renderer;
  bool running;
  int winWidth;
  int winHeight;
  float deltaTime;
};

SDLState initDrivers();
void handleSDLEvents(SDLState &state);
