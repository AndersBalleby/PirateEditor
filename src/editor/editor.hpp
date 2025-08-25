#pragma once

#include "ui.hpp"
#include <SDL3/SDL_render.h>
#include <vector>
class Editor {
private:
  SDL_Texture *tex;
  std::vector<SDL_FRect> clips = {};
public:
  Editor(SDL_Renderer *renderer);
  ~Editor();

  void run(SDL_Renderer *renderer, UI &ui, float deltaTime);
};
