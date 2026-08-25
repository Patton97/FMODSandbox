#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_studio.hpp>
#include <string>
#include <map>

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    void LoadBanksFromFolder(std::string folderPath);
    void PlayAudio(const char* eventPath);

    void Update();

private:
    FMOD::Studio::System* studioSystem = nullptr;
    std::map<std::string, FMOD::Studio::Bank*> banksKeyedByName;

    bool SucceededOrWarn(const std::string& message, FMOD_RESULT result);
};
