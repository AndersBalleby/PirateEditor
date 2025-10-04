#include "Background.hpp"
#include "SDL3/SDL_render.h"
#include "core/ResourceManager.hpp"
#include "logging/Logger.hpp"

Background::Background() {
  if(!init()) {
    Log::Error("Kunne ikke initialisere baggrund");
  }

  Log::Info("Baggrund initialiseret");
}

bool Background::init() {
  Log::Info("Initialiserer baggrund");
  sky_bottom = ResourceManager::loadTexture(SKY_BOTTOM_PATH);
  sky_middle = ResourceManager::loadTexture(SKY_MIDDLE_PATH);
  sky_top = ResourceManager::loadTexture(SKY_TOP_PATH);

  if(!sky_bottom || !sky_middle || !sky_top) {
    Log::Error("Kunne ikke indlæse baggrundsbilleder");
    return false;
  }

  return true;
}

void Background::render(SDL_Renderer *renderer) {
    // Render background

    // Render top
    SDL_RenderTexture(renderer, sky_top, nullptr, &sky_top_rect);

    // Render middle
    SDL_RenderTexture(renderer, sky_middle, nullptr, &sky_middle_rect);

    // Render bottom
    SDL_RenderTexture(renderer, sky_bottom, nullptr, &sky_bottom_rect);
}
