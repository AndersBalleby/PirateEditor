#include "SDL_Handler.hpp"

SDL_Handler::SDL_Handler(WindowConfig winConfig) {
  if(!initSDL()) return;

  window = SDL_CreateWindow(winConfig.title.c_str(), winConfig.width, winConfig.height, SDL_WINDOW_RESIZABLE);
  if(!window) {
    Log::Critical("Kunne ikke oprette vindue");
    return;
  }

  renderer = SDL_CreateRenderer(window, nullptr);
  if(!renderer) {
    Log::Critical("Kunne ikke oprette renderer");
    return;
  }

  if(!ResourceManager::init(renderer)) {
    Log::Critical("Fejl ved initialiseringen af ResourceManager");
    return;
  }

  if(!(font = loadFont("resources/ui/ARCADEPI.TTF", 20))) {
    Log::Critical("Fejl ved indlæsning af font");
    return;
  }

  // Gør at textures ikke er så dårlig opløsning
  SDL_SetDefaultTextureScaleMode(renderer, SDL_SCALEMODE_NEAREST);
  
  running = true;
  Log::Info("SDL_Handler er korrekt initialiseret");
}

SDL_Handler::~SDL_Handler() {
  cleanup();
}

bool SDL_Handler::initSDL() {
  if(!SDL_Init(SDL_INIT_VIDEO)) {
    Log::Critical("SDL_Init fejl");
    return false;
  }

  if(!TTF_Init()) {
    Log::Critical("TTF_Init fejl");
    return false;
  }

  return true;
}

void SDL_Handler::cleanup() {
  Log::Info("Afslutter alle subsystems...");
  
  // Alle resources skal cleares (frigøres) før renderer og window slettes
  ResourceManager::clear();

  if(renderer) SDL_DestroyRenderer(renderer);
  if(window)   SDL_DestroyWindow(window);
  if(font)     TTF_CloseFont(font);


  TTF_Quit();
  SDL_Quit();
}

bool SDL_Handler::isRunning() const {
  return running;
}

void SDL_Handler::clear() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
}

void SDL_Handler::present() {
  SDL_RenderPresent(renderer);
}

SDL_Renderer* SDL_Handler::getRenderer() const {
  return renderer;
}

TTF_Font* SDL_Handler::getFont() const {
  return font;
};

[[nodiscard]] SDL_Texture* SDL_Handler::loadTexture(const std::string& path) {
  SDL_Surface* surf = IMG_Load(path.c_str());
  if(!surf) {
    Log::Error("IMG_Load fejl");
    return nullptr;
  }

  SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_DestroySurface(surf);
  
  return tex;
}

[[nodiscard]] TTF_Font* SDL_Handler::loadFont(const std::string& path, size_t size) {
  TTF_Font* font = TTF_OpenFont(path.c_str(), size);
  if(!font) {
    Log::Error("TTF_OpenFont fejl");
    return nullptr;
  }

  return font;
}