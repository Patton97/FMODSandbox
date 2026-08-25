#include <chrono>
#include <conio.h>
#include <filesystem>
#include <iostream>
#include <map>
#include <thread>

#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_studio.hpp>

using recursive_directory_iterator = std::filesystem::recursive_directory_iterator;

bool succeededOrWarn(const std::string& message, FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        std::cerr << message << ": " << result << " " << FMOD_ErrorString(result) << std::endl;
        return false;
    }
    return true;
}

std::map<std::string, FMOD::Studio::Bank*> loadBanksFromFolder(FMOD::Studio::System* studioSystem, std::string folderPath)
{
    std::map<std::string, FMOD::Studio::Bank*> banksKeyedByName;
    std::string bankFileExtension(".bank");
    for (const auto& dirEntry : recursive_directory_iterator(folderPath))
    {
        if (dirEntry.path().extension() != bankFileExtension)
            continue;

        FMOD::Studio::Bank* bank;
        FMOD_RESULT result = studioSystem->loadBankFile(dirEntry.path().string().c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank);
        if (succeededOrWarn(std::string("Failed to load bank at path ").append(dirEntry.path().string()), result))
        {
            banksKeyedByName[dirEntry.path().string()] = bank;
            std::cout << "Successfully loaded bank at path " << dirEntry.path() << std::endl;
        }
    }

    return banksKeyedByName;
}

void playAudioAsync(FMOD::Studio::System* studioSystem, const char* eventPath)
{
    FMOD::Studio::EventDescription* eventDescription = nullptr;
    FMOD_RESULT result = studioSystem->getEvent(eventPath, &eventDescription);
    if (!succeededOrWarn(std::string("Failed to find event at path ").append(eventPath), result))
    {
        return;
    }

    FMOD::Studio::EventInstance* eventInstance = nullptr;
    result = eventDescription->createInstance(&eventInstance);
    if (!succeededOrWarn(std::string("Failed to create instance of event at path ").append(eventPath), result))
    {
        return;
    }

    eventInstance->start();

    FMOD_STUDIO_PLAYBACK_STATE state;
    do
    {
        studioSystem->update();
        eventInstance->getPlaybackState(&state);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    } while (state != FMOD_STUDIO_PLAYBACK_STOPPED);

    eventInstance->release();
}

int main(int argc, char* argv[])
{
    FMOD::Studio::System* studioSystem = nullptr;

    // Create the main system object.
    FMOD_RESULT result = FMOD::Studio::System::create(&studioSystem);
    if (!succeededOrWarn("FMOD: Failed to create system object", result))
        return 1;

    // Initialize FMOD.
    result = studioSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
    if (!succeededOrWarn("FMOD: Failed to initialise system object", result))
        return 1;

    std::map<std::string, FMOD::Studio::Bank*> banksKeyedByName = loadBanksFromFolder(studioSystem, "Assets/FMODBanks/");    

    std::thread playAudioAsyncTask(playAudioAsync, studioSystem, "event:/Ambience/Forest");
    playAudioAsyncTask.join();

    studioSystem->release();

    std::cout << "Hello World!\n";

    return 0;
}