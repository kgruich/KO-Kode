#include "AudioDB.h"

#include "lua.hpp"
#include "LuaBridge/LuaBridge.h"
#include "../actors/ComponentManager.h"

void AudioDB::init() {
    AudioHelper::Mix_OpenAudio498(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    AudioHelper::Mix_AllocateChannels498(50);
    loadAudios();
    initialized = true;
    instance = this;

    // Add relevent functions to Lua API
    luabridge::getGlobalNamespace(ComponentManager::luaState)
        .beginNamespace("Audio")
        .addFunction("Play", &playAPI)
        .addFunction("Halt", &haltAudio)
        .addFunction("SetVolume", &setVolume)
        .endNamespace();
}

AudioDB* AudioDB::getInstance()
{
    return instance;
}

void AudioDB::playAPI(const int channel, const std::string& audioName, const bool doesLoop) {
    if (doesLoop) {
        playAudio(channel, audioName, -1);
    }
    else {
        playAudio(channel, audioName, 0);
    }
}

void AudioDB::playAudio(const int channel, const std::string& audioName, const int loops) {
    if (!initialized) {
        return;
    }
    else if (audioName == "") {
        return;
    }
    else if (const auto& it = audios.find(audioName); it != audios.end()) {
        AudioHelper::Mix_PlayChannel498(channel, it->second, loops);
    }
    else {
        std::cout << "error: failed to play audio clip " + audioName;
        exit(0);
    }
}

void AudioDB::playSFX(const int channel, const std::string& audioName) {
    if (!initialized) {
        return;
    }
    else {
        playAudio(channel, audioName, 0);
    }
}

void AudioDB::playSFXRandomChannel(const std::string& audioName) {
    if (audioName != "") {
        int channel = Helper::GetFrameNumber() % 48 + 2;
        playSFX(channel, audioName);
    }
}

void AudioDB::playBGM(const std::string& audioName, const int loops) {
    if (!initialized) {
        return;
    }
    else {
        stopBGM();
        playingBGM = true;
        playAudio(0, audioName, loops);
    }
}

void AudioDB::stopBGM() {
    if (!initialized) {
        return;
    }
    else if (playingBGM) {
        playingBGM = false;
        AudioHelper::Mix_HaltChannel498(0);
    }
}

void AudioDB::setVolume(const int channel, const float volume) {
    if (!initialized) {
        return;
    }
    else {
        AudioHelper::Mix_Volume498(channel, static_cast<int>(volume));
    }
}

void AudioDB::haltAudio(const int channel) {
    if (!initialized) {
        return;
    }
    else {
        AudioHelper::Mix_HaltChannel498(channel);
    }
}

void AudioDB::loadAudios() {
    if (!std::filesystem::exists(audioPath)) {
        return;
    }
    for (const auto& file : std::filesystem::directory_iterator(audioPath)) {
        if (file.path().extension() == ".wav" || file.path().extension() == ".ogg") {
            std::string audioName = file.path().stem().string();
            std::string filePath = file.path().string();
            Mix_Chunk* raw_audio = AudioHelper::Mix_LoadWAV498(filePath.c_str());
            if (raw_audio == nullptr) {
                std::cout << "error: failed to load audio clip " + audioName;
                exit(0);
            }
            audios[audioName] = raw_audio;
        }
    }
}
