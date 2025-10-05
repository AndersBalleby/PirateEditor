#include "FPS_Counter.hpp"

FPS_Counter::FPS_Counter()
 : texture(nullptr)
 , now(SDL_GetTicks())
 , last(0)
 , fpsTimer(0.0)
 , fps(0)
 , frameCount(0)
{
  Log::Info("FPS_Counter oprettet");
}

void FPS_Counter::update(SDL_State& state) noexcept {

  frameCount++;
  if(fpsTimer >= 1.0) {
    fps = frameCount;
    frameCount = 0;
    fpsTimer = 0.0;

    updateText(state.renderer, state.font);
  }

  // Draw texture
  if(texture) {
    SDL_RenderTexture(state.renderer, texture, nullptr, &rect);
  }

  fpsTimer += state.deltaTime;
}

void FPS_Counter::updateText(SDL_Renderer* renderer, TTF_Font* font) noexcept {
  if(texture) SDL_DestroyTexture(texture);

  std::string fpsText = "FPS: " + std::to_string(fps);
  SDL_Color white = {255, 255, 255, 255};
  SDL_Surface* textSurface = TTF_RenderText_Blended(font, fpsText.c_str(), fpsText.size(), white);
  texture = SDL_CreateTextureFromSurface(renderer, textSurface);
  rect = {10.0, 10.0, (float) textSurface->w, (float) textSurface->h};

  SDL_DestroySurface(textSurface);
}

FPS_Counter::~FPS_Counter() {
  if(texture) {
    SDL_DestroyTexture(texture);
  }
}
