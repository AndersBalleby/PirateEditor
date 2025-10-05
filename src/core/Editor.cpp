#include "Editor.hpp"

Editor::Editor()
  : current_scene(0, "TestScene")
{
  Log::Info("Initialiserer editor");
}

void Editor::run(SDL_State& state) {
  current_scene.update(state.deltaTime, state.keyState);
  current_scene.draw(state.renderer);
}
