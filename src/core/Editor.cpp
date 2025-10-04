#include "Editor.hpp"

Editor::Editor()
  : current_scene(0, "TestScene")
{
  Log::Info("Initialiserer editor");
}

void Editor::run(SDL_Renderer* renderer, float deltaTime) {
  current_scene.update(deltaTime);
  current_scene.draw(renderer);
}
