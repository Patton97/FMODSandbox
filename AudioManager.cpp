#include "AudioManager.h"

#include <iostream>
#include <filesystem>
#include <map>
#include <thread>

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

AudioManager::AudioManager()
{
    FMOD_RESULT result = FMOD::Studio::System::create(&this->studioSystem);

    if (!this->SucceededOrWarn("FMOD: Failed to create system object", result))
        return;

    result = studioSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
    if (!this->SucceededOrWarn("FMOD: Failed to initialise system object", result))
        return;
}

AudioManager::~AudioManager()
{
    this->studioSystem->release();
    delete this->studioSystem;
    this->studioSystem = nullptr;
}

void AudioManager::LoadBanksFromFolder(std::string folderPath)
{
    std::string bankFileExtension(".bank");
    for (const auto& dirEntry : recursive_directory_iterator(folderPath))
    {
        if (dirEntry.path().extension() != bankFileExtension)
            continue;

        FMOD::Studio::Bank* bank;
        FMOD_RESULT result = this->studioSystem->loadBankFile(dirEntry.path().string().c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
        if (this->SucceededOrWarn(std::string("Failed to load bank at path ").append(dirEntry.path().string()), result))
        {
            this->banksKeyedByName[dirEntry.path().string()] = bank;
            std::cout << "Successfully loaded bank at path " << dirEntry.path() << std::endl;
        }
    }

    RegisterBus("bus:/", masterBusController);
    RegisterBus("bus:/SFX/Explosions", sfxExplosionsBusController);
}

FMOD::Studio::EventInstance* AudioManager::PlayAudio(const char* eventPath)
{
    FMOD::Studio::EventDescription* eventDescription = nullptr;
    FMOD_RESULT result = studioSystem->getEvent(eventPath, &eventDescription);
    if (!this->SucceededOrWarn(std::string("Failed to find event at path ").append(eventPath), result))
        return nullptr;

    FMOD::Studio::EventInstance* eventInstance = nullptr;
    result = eventDescription->createInstance(&eventInstance);
    if (!this->SucceededOrWarn(std::string("Failed to create instance of event at path ").append(eventPath), result))
        return nullptr;

    eventInstance->start();
    eventInstance->release();

    return eventInstance;
}

void AudioManager::Update()
{
    this->studioSystem->update();
}

FMODBusController* AudioManager::GetMasterBusController()
{
    return this->masterBusController;
}

FMODBusController* AudioManager::GetSFXExplosionsBusController()
{
    return this->sfxExplosionsBusController;
}

FMODBusController* AudioManager::GetBusController(int index)
{
    return this->busControllers[index];
}

int AudioManager::GetBusCount()
{
    return this->busControllers.size();
}

void AudioManager::RegisterBus(const char* busPath, FMODBusController*& busStorage)
{
    FMOD::Studio::Bus* bus;
    FMOD_RESULT result = this->studioSystem->getBus(busPath, &bus);
    if (!this->SucceededOrWarn(std::string("FMOD: Failed to find bus at path ").append(busPath), result))
        return;

    busStorage = new FMODBusController(bus);
    this->busControllers.push_back(busStorage);
}

bool AudioManager::SucceededOrWarn(const std::string& message, FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        std::cerr << message << ": " << result << " " << FMOD_ErrorString(result) << std::endl;
        return false;
    }
    return true;
}