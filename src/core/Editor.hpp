#pragma once

#include "logging/Logger.hpp"
#include "scene/Scene.hpp"

class Editor {
  public:
    Editor();
    ~Editor();

    void run(SDL_Renderer* renderer);

    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;
  private:
    Scene current_scene;
};