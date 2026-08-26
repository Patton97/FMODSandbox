#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_studio.hpp>
#include <string>
#include <map>
#include "FMODBusController.h"
#include <vector>

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    void LoadBanksFromFolder(std::string folderPath);
    FMOD::Studio::EventInstance* PlayAudio(const char* eventPath);

    void Update();

    FMODBusController* GetMasterBusController();
    FMODBusController* GetSFXExplosionsBusController();

    FMODBusController* GetBusController(int index);
    int GetBusCount();

private:
    FMOD::Studio::System* studioSystem = nullptr;
    std::map<std::string, FMOD::Studio::Bank*> banksKeyedByName;

    std::vector<FMODBusController*> busControllers;
    FMODBusController* masterBusController;
    FMODBusController* sfxExplosionsBusController;

    void RegisterBus(const char* busPath, FMODBusController*& busStorage);
    bool SucceededOrWarn(const std::string& message, FMOD_RESULT result);
};
