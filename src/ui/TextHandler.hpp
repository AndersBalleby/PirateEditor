#pragma once

#include <string>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <unordered_map>
#include <chrono>

#include "logging/Logger.hpp"
#include "math/vec.hpp"

namespace UI {

using Clock = std::chrono::steady_clock;

struct ColoredSegment {
    std::string text;
    SDL_Color color;
};

struct CachedText {
  SDL_Texture* texture;
  Clock::time_point lastUsed;
};

struct TextKey {
  std::string text;
  SDL_Color color;

  bool operator==(const TextKey& other) const {
      return text == other.text &&
        color.r == other.color.r &&
        color.g == other.color.g &&
        color.b == other.color.b &&
        color.a == other.color.a;
    };
};

struct TextKeyHash {
  std::size_t operator()(const TextKey& k) const {
    std::size_t h1 = std::hash<std::string>()(k.text);
    std::size_t h2 = (k.color.r << 24) | (k.color.g << 16) | (k.color.b << 8) | k.color.a;
    return h1 ^ (h2 * 0x9e3779b9);
  }
};

class Text {
  public:
    static bool init(SDL_Renderer* renderer, const std::string& fontPath, float fontSize);
    static void cleanup();

    static void displayText(const std::string& text, Vec2<float> position, SDL_Color color = {255, 255, 255, 255});
    static void trimUnusedCache(double maxAgeSeconds = 5.0);
  private:
    Text() = default;

    static std::vector<ColoredSegment> parseColoredText(const std::string& input, SDL_Color defaultColor = {255, 255, 255, 255});

    static SDL_Renderer* s_renderer;
    static TTF_Font* s_font;

    static std::unordered_map<TextKey, CachedText, TextKeyHash> s_cache;
    static std::unordered_map<std::string, SDL_Color> s_colorMap;
};

};
