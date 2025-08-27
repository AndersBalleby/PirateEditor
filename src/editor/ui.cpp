#include "ui.hpp"
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_timer.h>

/*###################################*/
/*                                   */
/*           FPS COUNTER             */
/*                                   */
/*###################################*/
FPSCounter::FPSCounter(SDL_Renderer *renderer, TextRenderer *tRenderer)
    : renderer(renderer), tRenderer(tRenderer),
      text(renderer, tRenderer->getFont(), "FPS: 0", 1.0f, color),
      prevCounter(SDL_GetPerformanceCounter()),
      freq(SDL_GetPerformanceFrequency()) {}

void FPSCounter::update(float deltaTime) {
  fpsDeltaTimer += deltaTime;

  if (fpsDeltaTimer >= 0.5) {
    fps = (deltaTime > 0.0) ? (int)(1.0 / deltaTime) : 0;
    std::snprintf(buffer, sizeof(buffer), "FPS: %d", fps);
    this->text.updateMessage(this->renderer, this->tRenderer->getFont(), buffer,
                             color);
    fpsDeltaTimer = 0.0;
  }
}

void FPSCounter::draw() const {
  tRenderer->displayText(text, position.x, position.y);
}

/*###################################*/
/*                                   */
/*               UI                  */
/*                                   */
/*###################################*/
UI::UI(SDL_Renderer *renderer, TextRenderer *tRenderer)
  : fpsCounter(renderer, tRenderer)
{}

void UI::update(float deltaTime, int mouseX) {
  // Update FPS counter
  this->fpsCounter.update(deltaTime);

  float targetWidth = sidebar.widthClosed;
  if (mouseX < sidebar.width) {
    targetWidth = sidebar.widthOpen;
  }

  if (sidebar.width < targetWidth) {
    sidebar.width += sidebar.speed * deltaTime;
    if (sidebar.width > targetWidth) {
      sidebar.width = targetWidth;
    }
  } else if (sidebar.width > targetWidth) {
    sidebar.width -= sidebar.speed * deltaTime;
    if (sidebar.width < targetWidth)
      sidebar.width = targetWidth;
  }
  
  sidebar.expanded = (sidebar.width == sidebar.widthOpen);
}

void UI::draw(SDL_Renderer *renderer) const {
  SDL_SetRenderDrawColor(renderer, sidebar.backgroundColor.r,
                         sidebar.backgroundColor.g, sidebar.backgroundColor.b,
                         sidebar.backgroundColor.a);
  SDL_FRect sidebarRect = {0, 0, (float)sidebar.width, 720};
  SDL_RenderFillRect(renderer, &sidebarRect);

  this->fpsCounter.draw();
}
