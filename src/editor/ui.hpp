#pragma once

#include "../core/TextRenderer.hpp"
#include "../core/mathlib.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_timer.h>
#include <cmath>
#include <cstdio>

struct Sidebar {
  const float widthClosed = 50.0f;
  const float widthOpen = 200.0f;
  const float speed = 500.0f;
  const SDL_Color backgroundColor = {50, 50, 50, 255};
  float width = 50.0f;
  bool expanded = false;
};

struct FPSCounter {
  const Vec2 position{1024-125, 10};
  const SDL_Color color = {255, 255, 255, SDL_ALPHA_OPAQUE};
  SDL_Renderer *renderer = nullptr;
  TextRenderer *tRenderer = nullptr;
  Text text;
  char buffer[128] = {};
  uint64_t prevCounter = 0;
  uint64_t freq = 0;
  double fpsDeltaTimer = 0.0;
  int fps = 0;

  FPSCounter(SDL_Renderer *renderer, TextRenderer *tRenderer);
  void update(float deltaTime);
  void draw() const;
};

class UI {
private:
  Sidebar sidebar;
  FPSCounter fpsCounter;
public:
  UI(SDL_Renderer *renderer, TextRenderer *tRenderer);
  ~UI() = default;
  
  const Sidebar &getSidebar() const { return sidebar; }
  void update(float deltaTime, int mouseX);
  void draw(SDL_Renderer *renderer) const;
};
