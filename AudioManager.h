#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_studio.hpp>
#include <string>
#include <map>
#include <vector>

#include "BusController.h"
#include "EventInstanceController.h"

class AudioManager
{
public:
    AudioManager();
    ~AudioManager();

    void LoadBanksFromFolder(std::string folderPath);
    EventInstanceController* PlayAudio(const char* eventPath);

    void Update();

    BusController* GetMasterBusController();
    BusController* GetSFXExplosionsBusController();

    BusController* GetBusController(int index);
    int GetBusCount();

private:
    FMOD::Studio::System* studioSystem = nullptr;
    std::map<std::string, FMOD::Studio::Bank*> banksKeyedByName;

    std::vector<BusController*> busControllers;
    BusController* masterBusController;
    BusController* sfxExplosionsBusController;

    void RegisterBus(const char* busPath, BusController*& busStorage);
    bool SucceededOrWarn(const std::string& message, FMOD_RESULT result);
};
