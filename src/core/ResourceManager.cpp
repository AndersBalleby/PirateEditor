#include "ResourceManager.hpp"
#include "logging/Logger.hpp"
#include <__format/format_functions.h>


SDL_Renderer* ResourceManager::s_renderer = nullptr;
std::unordered_map<std::string, SDL_Texture*> ResourceManager::s_textures;
std::unordered_map<std::string, std::unique_ptr<Animation>> ResourceManager::s_animations;

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

SDL_Texture* ResourceManager::loadTileMap(const std::string& path) {
  SDL_Texture* mapTex = loadTexture(path);
  if(!mapTex) {
    Log::Critical("Kunne ikke indlæse tilemap: {}", path.c_str());
    return nullptr;
  }

  return mapTex;
}

[[nodiscard]] Animation* ResourceManager::loadAnimation(const std::string& animID, const std::filesystem::path& folderPath) {
    auto animation = std::make_unique<Animation>(animID, folderPath);
    if(animation->getTextures().empty()) {
        return nullptr;
    }

    s_animations[animID] = std::move(animation);
    return s_animations[animID].get();
}

Animation* ResourceManager::getAnimation(const std::string &animID) {
  auto it = s_animations.find(animID);
  return (it != s_animations.end()) ? it->second.get() : nullptr;
}

void ResourceManager::clear() {
  for(auto &kv : s_textures) {
    Log::Info("Slettede resource: {}", kv.first.c_str());
    SDL_DestroyTexture(kv.second);
  }

  s_textures.clear();
  s_renderer = nullptr;
}

void ResourceManager::update(float deltaTime) {
  for(const auto& [animID, anim] : s_animations) {
    anim->tick(deltaTime);
  }
}

Animation::Animation(const std::string& animID, const std::filesystem::path& folderPath) {
  std::vector<std::filesystem::path> files = Utils::getAnimationFiles(folderPath);
  if(files.size() == 0) {
    Log::Error("Kunne ikke oprette animationen \"{}\" ({})", animID, folderPath.string());
    return;
  }

  for(const auto& file : files) {
    SDL_Texture* tex = ResourceManager::loadTexture(file);
    if(!tex) {
      Log::Critical("Kunne ikke fuldføre animationen \"{}\", problem med filen \"{}\"", animID, file.string());
      return;
    }

    textures.push_back(ResourceManager::loadTexture(file));
  }

  Log::Info("Indlæste alle textures for animation \"{}\"", animID);
}

void Animation::tick(float deltaTime) {
  current_frame += ANIMATION_SPEED * deltaTime;
  if(current_frame >= textures.size()) {
    current_frame = 0;
  }
}

void Animation::draw(SDL_Renderer* renderer, SDL_FRect* srcRect, SDL_FRect* destRect) {
  SDL_RenderTexture(renderer, textures[(int) current_frame], srcRect, destRect);
}

float Animation::getCurrentFrame() const {
  return current_frame;
}

SDL_Texture* Animation::getCurrentTexture() const {
  return textures[(int) current_frame];
}

std::vector<SDL_Texture*> Animation::getTextures() {
  return textures;
}

/* Højden af nuværende texture */
int Animation::getHeight() const {
  return getCurrentTexture()->h;
}

/* Bredde af nuværende texture */
int Animation::getWidth() const {
  return getCurrentTexture()->w;
}
