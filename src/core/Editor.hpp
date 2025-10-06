#pragma once

#include "logging/Logger.hpp"
#include "scene/Scene.hpp"
#include "sdl/SDL_Handler.hpp"

class Editor {
  public:
    Editor();
    ~Editor() = default;

    void run(SDL_State& state);
    void update(SDL_State& state);
    void draw(SDL_Renderer* renderer) const;

    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;
  private:
    Scene::Manager scene_manager;
};
