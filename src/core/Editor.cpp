#include "Editor.hpp"

Editor::Editor() 
  : current_scene(("TestScene")) 
{
  Log::Info("Initialiserer editor");
}

Editor::~Editor() {}

void Editor::run() {
  current_scene.update();
  current_scene.draw();
}