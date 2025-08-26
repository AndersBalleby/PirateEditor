#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

struct Text {
  SDL_Texture *texture;
  std::string message;
  SDL_Color color;
  float scale;

  Text(SDL_Renderer *renderer, TTF_Font *font, const std::string& message, const float scale, const SDL_Color color);
  ~Text();

  void updateMessage(SDL_Renderer *renderer, TTF_Font* font, const std::string &message, SDL_Color);
};

class TextRenderer {
private:
  SDL_Renderer *renderer;
  TTF_Font *font;
  float ptsize;
  bool initialised;
public:
  TextRenderer(SDL_Renderer *renderer, const std::string& font_path, float ptsize);
  ~TextRenderer();

  void displayText(const Text &text, float x, float y);
  bool isInitialised() { return this->initialised && font != nullptr; }
  TTF_Font *getFont() { return this->font; } 
};
