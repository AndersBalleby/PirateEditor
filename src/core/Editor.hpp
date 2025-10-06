#pragma once

#include "logging/Logger.hpp"
#include "scene/Scene.hpp"
#include "sdl/SDL_Handler.hpp"

class Editor {
  public:
    Editor();
    ~Editor() = default;

    void run(SDL_State& state);

    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;
  private:
    Scene::Manager scene_manager;
};
