#pragma once

#include <string>
#include "logging/Logger.hpp"

class Scene {
  public:
    Scene(const std::string& name);
    ~Scene();

    void update();
    void draw();
    
    void saveScene(const std::string& path);

  private:
    std::string name;
    std::string file_path;
};