#pragma once

#include <SDL3/SDL_render.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <string>

class Text {
private:
  SDL_Texture *texture = nullptr;
  std::string message;
  SDL_Color color;
  float scale;
private:
  void createTexture(SDL_Renderer *renderer, TTF_Font *font, const std::string &message, SDL_Color color);
public:
  Text(SDL_Renderer *renderer, TTF_Font *font, const std::string &message, float scale, SDL_Color color);
  ~Text();
  
  // Ingen kopier
  Text(const Text&) = delete;
  Text &operator=(const Text&) = delete;

  // Flytbar
  Text(Text &&other) noexcept;
  Text &operator=(Text &&other) noexcept;


  void updateMessage(SDL_Renderer *renderer, TTF_Font *font, const std::string &newMessage, SDL_Color newColor);
  SDL_Texture *getTexture() const { return texture; }
  float getScale() const { return scale; }
  const std::string &getMessage() const { return message; }
  SDL_Color getColor() const { return color; }
};

class TextRenderer {
private:
  SDL_Renderer *renderer = nullptr;
  TTF_Font *font = nullptr;
  bool initialised = false;
public:
  TextRenderer(SDL_Renderer *renderer, const std::string& fontPath, float ptsize);
  ~TextRenderer();

  void displayText(const Text &text, float x, float y) const;
  
  bool isInitialised() const { return initialised && font != nullptr; }
  TTF_Font *getFont() const { return font; } 
};
