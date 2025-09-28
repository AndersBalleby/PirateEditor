#pragma once

#include "logging/Logger.hpp"
#include "scene/Scene.hpp"

class Editor {
  public:
    Editor();
    ~Editor();

    void run();

    Editor(const Editor&) = delete;
    Editor& operator=(const Editor&) = delete;
  private:
    Scene current_scene;
};