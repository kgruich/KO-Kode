#ifndef AUDIODB_H
#define AUDIODB_H

#include <memory>
#include <unordered_map>

#include "SDL2/SDL.h"
#include <string>
#include "../../external_helpers/AudioHelper.h"

class AudioDB
{
public:
    AudioDB() = default;

    void init();

    static AudioDB* getInstance();

    static void playAPI(const int channel, const std::string& audioName, const bool doesLoop);

    static void playAudio(const int channel, const std::string& audioName, const int loops);

    static void playSFX(const int channel, const std::string& audioName);

    static void playSFXRandomChannel(const std::string& audioName);

    static void playBGM(const std::string& audioName, const int loops);

    static void stopBGM();

    static void setVolume(const int channel, const float volume);

    static void haltAudio(const int channel);
private:
    static inline AudioDB* instance = nullptr;
    static inline bool initialized = false;
    static inline bool playingBGM = false;

    static inline std::string audioPath = "resources/audio/";
    static inline std::unordered_map<std::string, Mix_Chunk*> audios = {};

    static void loadAudios();
};
#endif // AUDIODB_H