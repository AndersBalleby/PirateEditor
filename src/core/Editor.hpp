#pragma once

#include "logging/Logger.hpp"
#include "scene/Scene.hpp"

class Editor {
  public:
    Editor();
    ~Editor() = default;

    void run(SDL_Renderer* renderer, float deltaTime);

    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;
  private:
    Scene current_scene;
};
