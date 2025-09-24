#include "SDL_Handler.hpp"

SDL_Handler::SDL_Handler(int width, int height, const std::string& title) {
  if(!initSDL()) return;

  window = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE);
  if(!window) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "Kunne ikke oprette vindue: %s", SDL_GetError());
    return;
  }

  renderer = SDL_CreateRenderer(window, nullptr);
  if(!renderer) {
    SDL_LogCritical(SDL_LOG_CATEGORY_RENDER, "Kunne ikke oprette renderer: %s", SDL_GetError());
    return;
  }

  running = true;
  SDL_LogInfo(SDL_LOG_CATEGORY_APPLICATION, "SDL_Handler er korrekt initialiseret");
}

SDL_Handler::~SDL_Handler() {
  cleanup();
}

bool SDL_Handler::initSDL() {
  if(!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "SDL_Init fejl: %s", SDL_GetError());
    return false;
  }

  if(!TTF_Init()) {
    SDL_LogCritical(SDL_LOG_CATEGORY_APPLICATION, "TTF_Init fejl: %s", SDL_GetError());
    return false;
  }

  return true;
}

void SDL_Handler::cleanup() {
  if(renderer) SDL_DestroyRenderer(renderer);
  if(window)   SDL_DestroyWindow(window);
  
  TTF_Quit();
  SDL_Quit();
}

bool SDL_Handler::isRunning() const {
  return running;
}

void SDL_Handler::clear() {
  SDL_RenderClear(renderer);
}

void SDL_Handler::present() {
  SDL_RenderPresent(renderer);
}

SDL_Renderer* SDL_Handler::getRenderer() const {
  return renderer;
}

SDL_Texture* SDL_Handler::loadTexture(const std::string& path) {
  SDL_Surface* surf = IMG_Load(path.c_str());
  if(!surf) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "IMG_Load fejl: %s", SDL_GetError());
    return nullptr;
  }

  SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_DestroySurface(surf);
  
  return tex;
}

TTF_Font* SDL_Handler::loadFont(const std::string& path, size_t size) {
  TTF_Font* font = TTF_OpenFont(path.c_str(), size);
  if(!font) {
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "TTF_OpenFont fejl: %s", SDL_GetError());
    return nullptr;
  }

  return font;
}