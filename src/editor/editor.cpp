#include "editor.hpp"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_image/SDL_image.h>
#include <iostream>
#include <ostream>
#include <sys/types.h>

// Igang med at afprøve spritesheet funktionalitet
Editor::Editor(SDL_Renderer *renderer) {
  SDL_Surface *spritesheet_surf = IMG_Load("test_assets/terrain.png");
  if (!spritesheet_surf) {
    std::cerr << "Kunne ikke indlæse spritesheet" << std::endl;
    return;
  }


  this->tex = SDL_CreateTextureFromSurface(renderer, spritesheet_surf);
  SDL_DestroySurface(spritesheet_surf);

  if(!this->tex) {
    std::cerr << "Kunne ikke lave texture" << std::endl;
    return;
  }

  float texW, texH;
  SDL_GetTextureSize(this->tex, &texW, &texH);
  
  for(int y = 0; y < texH; y += 32) {
    for(int x = 0; x < texW; x += 32) {
      this->clips.push_back({(float) x, (float) y, 32.0f, 32.0f});
    }
  }
}

Editor::~Editor() {
  SDL_DestroyTexture(this->tex);
}

void Editor::run(SDL_Renderer *renderer, UI &ui, float deltaTime) {
  float mx, my;
  SDL_GetMouseState(&mx, &my);
  float lx, ly;
  SDL_RenderCoordinatesFromWindow(renderer, mx, my, &lx, &ly);

  int gridSize = 32;
  int gridX = (int)(lx / gridSize);
  int gridY = (int)(ly / gridSize);

  SDL_FRect highlightRect;
  highlightRect.x = gridX * gridSize;
  highlightRect.y = gridY * gridSize;
  highlightRect.w = gridSize;
  highlightRect.h = gridSize;
  
  if(highlightRect.x > ui.getSidebar().width) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 100);
    SDL_RenderFillRect(renderer, &highlightRect);
  }
  
  ui.update(deltaTime, (int) lx);
  ui.draw(renderer);

}
