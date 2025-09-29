#include "ResourceManager.hpp"

SDL_Renderer* ResourceManager::s_renderer = nullptr;
std::unordered_map<std::string, SDL_Texture*> ResourceManager::s_textures;

bool ResourceManager::init(SDL_Renderer* renderer) {
  if(renderer == nullptr) {
    return false;
  }

  s_renderer = renderer;
  return true;
}

[[nodiscard]] SDL_Texture* ResourceManager::loadTexture(const std::string& path) {
  if(!s_renderer) {
    Log::Critical("ResourceManager er ikke initialiseret!");
    return nullptr;
  }

  auto it = s_textures.find(path);
  if(it != s_textures.end()) {
    return it->second; // Denne texture er allerede loadet
  }

  SDL_Surface* surface = IMG_Load(path.c_str());
  if(!surface) {
    Log::Critical("Kunne ikke indlæse billedet: {}", path.c_str());
    return nullptr;
  }

  SDL_Texture* tex = SDL_CreateTextureFromSurface(s_renderer, surface);
  SDL_DestroySurface(surface);

  if(!tex) {
    Log::Critical("Kunne ikke oprette texture fra surface: {}", path.c_str());
    return nullptr;
  }

  Log::Info("Indlæste resource: {}", path.c_str());
  s_textures[path] = tex;
  return tex;
}

void ResourceManager::clear() {
  for(auto &kv : s_textures) {
    Log::Info("Slettede resource: {}", kv.first.c_str());
    SDL_DestroyTexture(kv.second);
  }

  s_textures.clear();
  s_renderer = nullptr;
}

/* ANIMATIONS */
Animation::Animation(const std::string& animID, const std::filesystem::path& folderPath) {
  std::vector<std::filesystem::path> files = Utils::getAnimationFiles(folderPath);
  if(files.size() == 0) {
    Log::Error("Kunne ikke oprette animationen \"{}\" ({})", animID, folderPath.string());
    return;
  }

  // Fortsæt videre her
}

void Animation::tick(float deltaTime) {}