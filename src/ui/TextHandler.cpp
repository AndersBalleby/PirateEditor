#include "TextHandler.hpp"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_render.h"
#include "SDL3_ttf/SDL_ttf.h"

namespace UI {

  SDL_Renderer* Text::s_renderer = nullptr;
  TTF_Font* Text::s_font = nullptr;
  std::unordered_map<TextKey, CachedText, TextKeyHash> Text::s_cache;
  std::unordered_map<std::string, SDL_Color> Text::s_colorMap = {
      {"white", {255, 255, 255, 255}},
      {"red",   {255, 0, 0, 255}},
      {"green", {0, 255, 0, 255}},
      {"blue",  {0, 0, 255, 255}},
      {"yellow",{255, 255, 0, 255}},
      {"cyan",  {0, 255, 255, 255}},
      {"magenta",{255, 0, 255, 255}},
      {"gray",  {128, 128, 128, 255}}
  };

  std::vector<ColoredSegment> Text::parseColoredText(const std::string& input, SDL_Color defaultColor) {
    std::vector<ColoredSegment> segments;
    SDL_Color currentColor = defaultColor;

    size_t pos = 0;
    while (pos < input.size()) {
      if (input[pos] == '{') {
        size_t end = input.find('}', pos);
        if (end != std::string::npos) {
          std::string colorName = input.substr(pos + 1, end - pos - 1);
          auto it = Text::s_colorMap.find(colorName);
          if (it != Text::s_colorMap.end()) {
            // Skift farve
            currentColor = it->second;
            pos = end + 1;
            continue;
          }
        }
      }

      // Find næste farvetag eller slutning
      size_t next = input.find('{', pos);
      std::string segmentText = input.substr(pos, next - pos);
      segments.push_back({segmentText, currentColor});
      if (next == std::string::npos) break;
      pos = next;
    }

    return segments;
  }

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

    auto segments = parseColoredText(text, color);
    float xOffset = 0.0f;

    for (const auto& seg : segments) {
      if (seg.text.empty()) continue;

      TextKey key{ seg.text, seg.color };
      SDL_Texture* texture = nullptr;
      auto now = Clock::now();

      auto it = s_cache.find(key);
      if (it != s_cache.end()) {
          texture = it->second.texture;
          it->second.lastUsed = now;
      } else {
          SDL_Surface* surface = TTF_RenderText_Blended(s_font, seg.text.c_str(), seg.text.length(), seg.color);
          if (!surface) continue;

          texture = SDL_CreateTextureFromSurface(s_renderer, surface);
          SDL_DestroySurface(surface);
          if (!texture) continue;

          s_cache[key] = { texture, now };
      }

      float w, h;
      SDL_GetTextureSize(texture, &w, &h);
      SDL_FRect destRect = { position.x + xOffset, position.y, w, h };
      SDL_RenderTexture(s_renderer, texture, nullptr, &destRect);

      xOffset += w; // flyt til næste segment
    }

    // eventuelt trim cache
    if (s_cache.size() > 30) trimUnusedCache(10.0);
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
