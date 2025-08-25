#pragma once

#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL.h>

class UI {
public:
  const float sidebarWidthClosed = 50.0f;
  const float sidebarWidthOpen = 200.0f;
  const float sidebarSpeed = 500.0f;
  float sidebarWidth = 50.0f;
  bool sidebarExpanded = false;
  SDL_Color sidebarBC = {50, 50, 50, 255};
public:
  UI();
  ~UI();
  void update(float deltaTime, int mouseX);
  void draw(SDL_Renderer *renderer);
};
