#include "TextRenderer.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>

/*###################################*/
/*              Text                 */
/*###################################*/
Text::Text(SDL_Renderer *renderer, TTF_Font *font, const std::string &message, float scale, SDL_Color color)
    : texture(nullptr), message(message), color(color), scale(scale) 
{
  createTexture(renderer, font, message, color);
}

Text::~Text() {
  if(texture) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }
}

Text::Text(Text &&other) noexcept
  : texture(other.texture)
  , message(std::move(other.message))
  , color(other.color)
  , scale(other.scale)
{
  other.texture = nullptr;
}

Text &Text::operator=(Text &&other) noexcept {
  if(this != &other) {
    if(texture) SDL_DestroyTexture(texture);
    texture = other.texture;
    message = std::move(other.message);
    color = other.color;
    scale = other.scale;
    other.texture = nullptr;
  }

  return *this;
}

void Text::updateMessage(SDL_Renderer *renderer, TTF_Font *font, const std::string &newMessage, SDL_Color newColor) {
  message = newMessage;
  color = newColor;
  createTexture(renderer, font, message, color);
}

void Text::createTexture(SDL_Renderer *renderer, TTF_Font *font, const std::string &message, SDL_Color color) {
  if(texture) {
    SDL_DestroyTexture(texture);
    texture = nullptr;
  }

  SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), 0, color);
  if(!surf) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "TTF_RenderText_Blended fejlede: %s", SDL_GetError());
    return;
  }

  texture = SDL_CreateTextureFromSurface(renderer, surf);
  SDL_DestroySurface(surf);

  if(!texture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "SDL_CreateTextureFromSurface fejlede: %s", SDL_GetError());
  }
}

/*###################################*/
/*          TextRenderer             */
/*###################################*/
TextRenderer::TextRenderer(SDL_Renderer *renderer, const std::string &fontPath, float ptsize)
  : renderer(renderer)
  , font(nullptr)
  , initialised(false)
{
  font = TTF_OpenFont(fontPath.c_str(), (int) ptsize);
  if(!font) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Kunne ikke indlæse font %s: %s", fontPath.c_str(), SDL_GetError());
    return;
  }

  initialised = true;
}

TextRenderer::~TextRenderer() {
  if(font) {
    TTF_CloseFont(font);
    font = nullptr;
  }
}

void TextRenderer::displayText(const Text &text, float x, float y) const {
  if(!text.getTexture()) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Forsøgte at display en null texture");
    return;
  }

  float texW, texH;
  SDL_GetTextureSize(text.getTexture(), &texW, &texH);

  SDL_FRect dst = { x, y, texW * text.getScale(), texH * text.getScale() };
  SDL_RenderTexture(renderer, text.getTexture(), nullptr, &dst);
}
