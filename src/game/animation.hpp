#pragma once

#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_render.h>
#include <string>
#include <vector>

class Animation {
private:
  std::string animID;
  std::string dirPath;
  std::vector<std::string> files;
  std::vector<SDL_Texture *> textures;
  int currentFrame;
  float animTimer;
  const float frameDuration = 0.1f; // 10 FPS

public:
  Animation(std::string animID, SDL_Renderer *renderer, std::string dirPath);
  ~Animation();
  
  void tick(float deltaTime);
  SDL_Texture *getCurrentTexture();
  std::vector<SDL_Texture *> getTextures() { return this->textures; }

private:
  static SDL_EnumerationResult dirCallback(void *userdata, const char *dirname, const char *basename);
  
};
