#include "ui.hpp"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>

UI::UI() {
}

UI::~UI() {}

void UI::update(float deltaTime, int mouseX) {
  float targetWidth = sidebarWidthClosed;
  if(mouseX < sidebarWidth) {
    targetWidth = sidebarWidthOpen;
  }

  if(sidebarWidth < targetWidth) {
    sidebarWidth += sidebarSpeed * deltaTime;
    if(sidebarWidth > targetWidth) {
      sidebarWidth = targetWidth;
    }  
  } else if(sidebarWidth > targetWidth) {
    sidebarWidth -= sidebarSpeed * deltaTime;
    if(sidebarWidth < targetWidth) sidebarWidth = targetWidth;
  }

  if(sidebarWidth == sidebarWidthOpen) {
    sidebarExpanded = true;
  } else {
    sidebarExpanded = false;
  }
}

void UI::draw(SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, sidebarBC.r, sidebarBC.g, sidebarBC.b, sidebarBC.a);
  SDL_FRect sidebarRect = {0, 0, (float) sidebarWidth, 720};
  SDL_RenderFillRect(renderer, &sidebarRect);
}
