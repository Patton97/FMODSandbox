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

    void IncreaseVolume(float increaseAmount = 0.1f);
    void DecreaseVolume(float decreaseAmount = 0.1f);
    void ModifyVolume(float modifyAmount = 0.1f);

    void SetVolume(float newVolume);

private:
    FMOD::Studio::System* studioSystem = nullptr;
    std::map<std::string, FMOD::Studio::Bank*> banksKeyedByName;

    bool SucceededOrWarn(const std::string& message, FMOD_RESULT result);
};
