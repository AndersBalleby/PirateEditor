#include "Editor.hpp"

Editor::Editor() 
  : current_scene(0, "TestScene") 
{
  Log::Info("Initialiserer editor");
}

Editor::~Editor() {}

void Editor::run(SDL_Renderer* renderer) {
  current_scene.update();
  current_scene.draw(renderer);
}