#include "TextHandler.hpp"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"

namespace UI {

  SDL_Renderer* Text::s_renderer = nullptr;
  TTF_Font* Text::s_font = nullptr;
  std::unordered_map<TextKey, CachedText, TextKeyHash> Text::s_cache;


  bool Text::init(SDL_Renderer *renderer, const std::string &fontPath, float fontSize) {
    if(s_renderer) return false;

    s_renderer = renderer;
    s_font = TTF_OpenFont(fontPath.c_str(), fontSize);
    return s_font != nullptr;
  }

  void Text::cleanup() {
    for(auto& pair : s_cache) {
      SDL_DestroyTexture(pair.second.texture);
    }
    s_cache.clear();

    if(s_font) {
      TTF_CloseFont(s_font);
      s_font = nullptr;
    }
  }

  void Text::displayText(const std::string &text, Vec2<float> position, SDL_Color color) {
    if(!s_renderer || !s_font) return;

    TextKey key{ text, color };
    SDL_Texture* texture = nullptr;

    auto now = Clock::now();
    auto it = s_cache.find(key);

    if(it != s_cache.end()) {
      texture = it->second.texture;
      it->second.lastUsed = now;
    } else {
      SDL_Surface* surface = TTF_RenderText_Blended(s_font, text.c_str(), text.length(), color);
      if(!surface) return;

      SDL_Texture* texture = SDL_CreateTextureFromSurface(s_renderer, surface);
      SDL_DestroySurface(surface);
      if(!texture) return;

      s_cache[key] = { texture, now };

      if(s_cache.size() >= 30) {
        trimUnusedCache(10.0);
      }
    }

    float w, h;
    SDL_GetTextureSize(texture, &w, &h);
    SDL_FRect destRect = {position.x, position.y, w, h};

    SDL_RenderTexture(s_renderer, texture, nullptr, &destRect);
  }

  void Text::trimUnusedCache(double maxAgeSeconds) {
    auto now = Clock::now();
    size_t removed = 0;

    for(auto it = s_cache.begin(); it != s_cache.end();) {
      double age = std::chrono::duration<double>(now - it->second.lastUsed).count();
      if(age > maxAgeSeconds) {
        SDL_DestroyTexture(it->second.texture);
        it = s_cache.erase(it);
        ++removed;
      } else {
        ++it;
      }
    }

    if(removed > 0) {
      Log::Info("Ryddede cache op: {} gamle textures fjernet", removed);
    }
  }

}
