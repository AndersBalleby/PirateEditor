#include "animation.hpp"
#include "../core/gamelib.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_filesystem.h>
#include <SDL3/SDL_render.h>
#include <SDL3_image/SDL_image.h>
#include <algorithm>
#include <iostream>

Animation::Animation(std::string animID, SDL_Renderer *renderer, std::string dirPath) 
  : dirPath(dirPath), currentFrame(0), animTimer(0.0f) {
  std::cout << "Creating animation {ID: " << animID << ", dirPath: " << dirPath << "}" << std::endl;

  if(SDL_GetPathInfo(dirPath.c_str(), nullptr)) {
    if(!SDL_EnumerateDirectory(dirPath.c_str(), dirCallback, &files)) {
      std::cerr << "Fejl ved directory enumeration: " << SDL_GetError() << std::endl;
    } else {
      std::sort(files.begin(), files.end(), natural_sort_cmp);

      for(std::string &path : files) {
        textures.push_back(IMG_LoadTexture(renderer, path.c_str()));
      }
    }
  } else {
    std::cerr << SDL_GetError() << std::endl;
  }
}

Animation::~Animation() {
  for(SDL_Texture *tex : textures) {
    SDL_DestroyTexture(tex);
  }
}

SDL_EnumerationResult Animation::dirCallback(void *userdata, const char *dirname, const char *basename) {
  auto files = static_cast<std::vector<std::string> *>(userdata);

  std::string fullPath = std::string(dirname) + std::string(basename);
  files->push_back(fullPath);

  return SDL_ENUM_CONTINUE;
}

SDL_Texture *Animation::getCurrentTexture() {
  return this->textures.at(currentFrame);
}

void Animation::tick(float deltaTime) {
  animTimer += deltaTime;
  if(animTimer >= frameDuration) {
    animTimer -= frameDuration;
    currentFrame++;
    if(currentFrame >= (int) textures.size()) {
      currentFrame = 0;
    }
  }
}
