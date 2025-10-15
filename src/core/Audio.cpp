#include "Audio.hpp"
#include "SDL3/SDL_audio.h"
#include "SDL3_mixer/SDL_mixer.h"
#include "logging/Logger.hpp"

AudioHandler::AudioHandler() {
  initialized = false;

  if(!MIX_Init()) {
    Log::Error("Kunne ikke initialisere SDL_mixer");
    return;
  }

  mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
  if(!mixer) {
    Log::Error("Kunne ikke oprette mixer");
    return;
  }

  track = MIX_CreateTrack(mixer);
  if(!track) {
    Log::Error("Kunne ikke oprette track");
    return;
  }

  overworld_audio = MIX_LoadAudio(mixer, "audio/overworld_music.wav", true);
  if(!overworld_audio) {
    Log::Error("Kunne ikke indlæse overworld musik");
    return;
  }

  MIX_SetMasterGain(mixer, master_gain);
  MIX_SetTrackAudio(track, overworld_audio);

  initialized = true;
}

void AudioHandler::destroy() {
  if(initialized) {
    MIX_DestroyTrack(track);
    MIX_DestroyAudio(overworld_audio);
    MIX_DestroyMixer(mixer);
    MIX_Quit();
  }
}

void AudioHandler::playTrack() const {
  if(!initialized) {
    Log::Error("AudioHandler ikke initialiseret");
    return;
  }

  if(!MIX_TrackPlaying(track)) {
    MIX_PlayTrack(track, -1);
  }
}

bool AudioHandler::isInitialized() const {
  return initialized;
}
