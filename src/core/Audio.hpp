#if defined(_WIN32) || defined(__linux__)

#pragma once

#include <SDL3_mixer/SDL_mixer.h>
#include "logging/Logger.hpp"

class AudioHandler {
public:
  AudioHandler();
  ~AudioHandler() = default;

  void destroy();

  void playTrack() const;
  bool isInitialized() const;

private:
  MIX_Mixer* mixer;
  MIX_Track* track;
  MIX_Audio* overworld_audio;

  float master_gain = 0.03f;
  bool initialized;
};

#endif
