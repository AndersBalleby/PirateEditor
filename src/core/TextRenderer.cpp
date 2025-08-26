#include "TextRenderer.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_surface.h>
#include <SDL3_ttf/SDL_ttf.h>

Text::Text(SDL_Renderer *renderer, TTF_Font *font, const std::string& message, const float scale, const SDL_Color color) {
  this->texture = nullptr;
  this->message = message;
  this->scale = scale;

  SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), 0, color);
  if(surf) {
    this->texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
  }
}

void Text::updateMessage(SDL_Renderer *renderer, TTF_Font* font, const std::string &message, SDL_Color color) {
  this->message = message;

  if(this->texture) {
    SDL_DestroyTexture(this->texture);
    this->texture = nullptr;
  }

  SDL_Surface *surf = TTF_RenderText_Blended(font, message.c_str(), 0, color);
  if(surf) {
    this->texture = SDL_CreateTextureFromSurface(renderer, surf);
    SDL_DestroySurface(surf);
  }
}

Text::~Text() {
  SDL_DestroyTexture(texture);
}

TextRenderer::TextRenderer(SDL_Renderer *renderer, const std::string& font_path, float ptsize) {
  this->initialised = false;
  this->renderer = renderer;
  if(!TTF_Init()) {
    SDL_Log("Kunne ikke initialisere SDL_ttf: %s\n", SDL_GetError());
    return; 
  }
  
  this->font = TTF_OpenFont(font_path.c_str(), ptsize);
  if(!this->font) {
    SDL_Log("Kunne ikke indlæse font: %s\n", SDL_GetError());
    return;
  }

  this->ptsize = ptsize;
  this->initialised = true;
}

TextRenderer::~TextRenderer() {
  if(this->font)
    TTF_CloseFont(font);
  
  if(this->initialised)
    TTF_Quit();
}

void TextRenderer::displayText(const Text &text, float x, float y) {
  if(!text.texture) {
    SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Texture fra tekst er null");
    return;
  }
  int w = 0, h = 0;
  SDL_FRect dst;

  SDL_GetRenderOutputSize(renderer, &w, &h);
  SDL_SetRenderScale(renderer, text.scale, text.scale);
  SDL_GetTextureSize(text.texture, &dst.w, &dst.h);
  dst.x = x;
  dst.y = y;

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderTexture(renderer, text.texture, NULL, &dst);
    
  SDL_SetRenderScale(renderer, 1.0f, 1.0f);
}
