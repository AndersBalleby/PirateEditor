#include "sdl/SDL_Handler.hpp"
#include "core/Editor.hpp"
#include "io/utils.hpp"

int main(void) {
  Log::Init();

  const int WINDOW_WIDTH = 1280;
  const int WINDOW_HEIGHT = 800;
  SDL_Handler sdl({
    .title = "PirateEditor",
    .width = WINDOW_WIDTH,
    .height = WINDOW_HEIGHT
  });

  Editor editor;
  uint64_t now = SDL_GetTicks();
  uint64_t last = 0;
  double deltaTime = 0.0;

  SDL_Texture* fpsTexture = nullptr;
  SDL_FRect fpsRect;
  double fpsTimer = 0.0;
  int frameCount = 0;
  int fps = 0;

  Animation test("player_idle", std::filesystem::path("resources/character/idle"));

  while(sdl.isRunning()) {
    last = now;
    now = SDL_GetTicks();

    deltaTime = (now - last) / 1000.0;

    SDL_Event event;
    while(SDL_PollEvent(&event)) {
      if(event.type == SDL_EVENT_QUIT) return 0;
    }

    sdl.clear();

    frameCount++;
    fpsTimer += deltaTime;
    if(fpsTimer >= 1.0) {
      fps = frameCount;
      frameCount = 0;
      fpsTimer = 0.0;

      if(fpsTexture) SDL_DestroyTexture(fpsTexture);

      std::string fpsText = "FPS: " + std::to_string(fps);
      SDL_Color white = {255, 255, 255, 255};
      SDL_Surface* textSurface = TTF_RenderText_Blended(sdl.getFont(), fpsText.c_str(), fpsText.size(), white);
      fpsTexture = SDL_CreateTextureFromSurface(sdl.getRenderer(), textSurface);
      fpsRect = {10.0, 10.0, (float) textSurface->w, (float) textSurface->h};
      Log::Info("{}, {}", (float) textSurface->w, (float) textSurface->h);
      SDL_DestroySurface(textSurface);
    }

    if(fpsTexture) {
      SDL_RenderTexture(sdl.getRenderer(), fpsTexture, nullptr, &fpsRect);
    }

    test.tick(deltaTime);
    
    SDL_RenderTexture(sdl.getRenderer(), test.getTextures()[(int) test.getCurrentFrame()], nullptr, nullptr);
    editor.run(sdl.getRenderer());

    sdl.present();
  }

  if(fpsTexture) SDL_DestroyTexture(fpsTexture);

  return 0;
}