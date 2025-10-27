#include "FPS_Counter.hpp"
#include "ui/TextHandler.hpp"

FPS_Counter::FPS_Counter()
 : now(SDL_GetTicks())
 , last(0)
 , fpsTimer(0.0)
 , fps(0)
 , frameCount(0)
{
  Log::Info("FPS_Counter oprettet");
}

void FPS_Counter::update(SDL_State& state) noexcept {

  frameCount++;
  fpsTimer += state.deltaTime; // deltaTime = tid siden sidste frame i sekunder

  if (fpsTimer >= 1.0) { // opdater FPS én gang per sekund
      fps = frameCount;   // antal frames på 1 sekund = FPS
      frameCount = 0;     // nulstil frame tæller
      fpsTimer -= 1.0;    // træk 1 sekund fra timeren (bevarer evt. overskydende tid)
  }

  // Vis FPS
  UI::Text::displayText("FPS: {green}" + std::to_string(int(fps)), Vec2<float>(10, 10));
}
