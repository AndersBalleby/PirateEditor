#include "SDL_Handler.hpp"

SDL_State SDL_Handler::state;

SDL_Handler::SDL_Handler(WindowConfig winConfig) {
  const char* version = "1.0.0";
  SDL_SetAppMetadata(winConfig.title.c_str(), version, winConfig.title.c_str());

  if(!initSDL()) return;

  SDL_DisplayID display = SDL_GetPrimaryDisplay();
  const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(display);
  const int WINDOW_WIDTH = mode->w;
  const int WINDOW_HEIGHT = mode->h;


  state.window = SDL_CreateWindow(winConfig.title.c_str(), WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);
  if(!state.window) {
    Log::Critical("Kunne ikke oprette vindue");
    return;
  }

  state.renderer = SDL_CreateRenderer(state.window, nullptr);
  SDL_SetRenderVSync(state.renderer, -1);
  if(!state.renderer) {
    Log::Critical("Kunne ikke oprette renderer");
    return;
  }

  if(!ResourceManager::init(state.renderer)) {
    Log::Critical("Fejl ved initialiseringen af ResourceManager");
    return;
  }

  if(!UI::Text::init(state.renderer, "resources/ui/ARCADEPI.TTF", {
  { UI::Text::FontType::Small, 16},
  { UI::Text::FontType::Medium, 20},
  { UI::Text::FontType::Large, 28},
  { UI::Text::FontType::Title, 36},
  })) {
    Log::Critical("Fejl ved indlæsning af font");
    return;
  }

  #if defined(_WIN32) || defined(__linux__)
  if(!state.audioHandler.isInitialized()) {
    Log::Critical("Kunne ikke oprette lyd");
    return;
  }
  #endif

  // Gør at textures ikke er så dårlig opløsning
  SDL_SetDefaultTextureScaleMode(state.renderer, SDL_SCALEMODE_NEAREST);

  state.windowHeight = winConfig.height;
  state.windowWidth = winConfig.width;
  state.running = true;
  state.keyState = SDL_GetKeyboardState(nullptr);

  SDL_SetWindowFullscreen(state.window, SDL_WINDOW_FULLSCREEN);

  Log::Info("SDL_Handler er korrekt initialiseret");
}

void SDL_State::tickDeltaTime(uint64_t now, uint64_t last) {
  deltaTime = (float)(now - last) / 1000.0f;
}

SDL_Handler::~SDL_Handler() {
  cleanup();
}

bool SDL_Handler::initSDL() {
  if(!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
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

  ResourceManager::clear();
  #if defined(_WIN32) || defined(__linux__)
  getAudioHandler().destroy();
  #endif

  if(state.renderer) SDL_DestroyRenderer(state.renderer);
  if(state.window)   SDL_DestroyWindow(state.window);
  UI::Text::cleanup();

  TTF_Quit();
  SDL_Quit();
}

bool SDL_Handler::isRunning() const {
  return state.running;
}

void SDL_Handler::clear() {
  SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
  SDL_RenderClear(state.renderer);
}

void SDL_Handler::present() {
  SDL_RenderPresent(state.renderer);
}

#if defined(_WIN32) || defined(__linux__)
AudioHandler& SDL_Handler::getAudioHandler() {
  return state.audioHandler;
}
#endif

SDL_Renderer* SDL_Handler::getRenderer() const {
  return state.renderer;
}


[[nodiscard]] SDL_Texture* SDL_Handler::loadTexture(const std::string& path) {
  SDL_Surface* surf = IMG_Load(path.c_str());
  if(!surf) {
    Log::Error("IMG_Load fejl");
    return nullptr;
  }

  SDL_Texture* tex = SDL_CreateTextureFromSurface(state.renderer, surf);
  SDL_DestroySurface(surf);

  return tex;
}

SDL_State& SDL_Handler::getState() {
  return state;
}
