#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <memory>
#include "../core/TextRenderer.hpp"

struct SDLState {
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::unique_ptr<TextRenderer> tRenderer;
  bool running;
  int winWidth;
  int winHeight;
  double deltaTime;
};

SDLState initDrivers();
void handleSDLEvents(SDLState &state);
