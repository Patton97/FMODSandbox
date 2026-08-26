#include <algorithm>
#include <chrono>
#include <conio.h>
#include <iostream>
#include <map>
#include <thread>

#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_studio.hpp>

#include "AudioManager.h"
#include "InputManager.h"

enum class ControlTarget
{
    Parameter,
    Volume,
};

int busControllerIndex = 0;
const float VOLUME_INCREMENT = 0.05f;
const float PARAM_INCREMENT = 0.05f;
EventInstanceController* forestAudioController;
std::map<char, std::string> forestParamsKeyedByKey;
std::string targetForestParam;
ControlTarget currentControlTarget = ControlTarget::Volume;

static int FloatToIntPct(float f)
{
    float rounded = std::round(f * 100.0) / 100.0;
    return (int)(rounded * 100);
}

static void PrintBusVolume(FMODBusController* busController)
{
    // output new volume as both feedback & a debug measure
    std::cout << busController->GetPath() << " | Volume = " << busController->GetVolume() << std::endl;
}

static void PrintForestAudioParameterValue(std::string parameterName, float newParamValue)
{
    std::cout << "Forest audio parameter '" << parameterName << "': " << FloatToIntPct(newParamValue) << "%" << std::endl;
}

static void ModifyVolume(FMODBusController* busController, float modifyAmount)
{
    busController->ModifyVolume(modifyAmount);

    // print new volume as both feedback & a debug measure
    PrintBusVolume(busController);
}

static void ModifyParameter(float modifyAmount)
{
    forestAudioController->ModifyParamValue(targetForestParam, modifyAmount);

    // print new value as both feedback & a debug measure
    PrintForestAudioParameterValue(targetForestParam, forestAudioController->GetParamValue(targetForestParam));
}

static void ModifyValue(AudioManager* audioManager, bool positiveModify)
{
    switch (currentControlTarget)
    {
        case ControlTarget::Parameter:
            ModifyParameter(positiveModify ? PARAM_INCREMENT : -PARAM_INCREMENT);
            break;

        case ControlTarget::Volume:
            ModifyVolume(audioManager->GetBusController(busControllerIndex), positiveModify ? VOLUME_INCREMENT : -VOLUME_INCREMENT);
            break;
    }
}

static void HandleInput(InputManager* inputManager, AudioManager* audioManager, bool* keepLooping)
{
    if (!_kbhit())
        return;

    KeyPress keyPress = inputManager->GetKeyPress();
    switch (keyPress.KeyCode)
    {
        case KeyCode::Unknown:
            std::cout << "Unknown key pressed!" << std::endl;
            break;

        case KeyCode::ArrowUp:
            ModifyValue(audioManager, /*positiveModify*/ true);
            break;

        case KeyCode::ArrowDown:
            ModifyValue(audioManager, /*positiveModify*/ false);
            break;

        case KeyCode::Character:
            FMODBusController* busController;
            switch (keyPress.Character)
            {
                case 'v':
                    currentControlTarget = ControlTarget::Volume;
                    // output new volume as both feedback & a debug measure
                    busController = audioManager->GetBusController(busControllerIndex);
                    std::cout << busController->GetPath() << " | Volume = " << busController->GetVolume() << std::endl;
                    break;

                default:
                    currentControlTarget = ControlTarget::Parameter;
                    targetForestParam = forestParamsKeyedByKey[keyPress.Character];

                    float currentParamValue = forestAudioController->GetParamValue(targetForestParam);
                    std::cout << "Forest audio parameter '" << targetForestParam << "': " << currentParamValue << std::endl;
                    break;
            }
            break;

        case KeyCode::Escape:
            *keepLooping = false;
            break;

        case KeyCode::Spacebar:
            audioManager->PlayAudio("event:/Weapons/Explosion");
            break;

        case KeyCode::Tab:
            busControllerIndex++;
            if (busControllerIndex >= audioManager->GetBusCount())
            {
                busControllerIndex = 0;
            }

            std::cout << "Selected bus: " << audioManager->GetBusController(busControllerIndex)->GetPath() << std::endl;
            break;
    }
}

int main(int argc, char* argv[])
{
    AudioManager audioManager;
    audioManager.LoadBanksFromFolder("Assets/FMODBanks/");
    forestAudioController = audioManager.PlayAudio("event:/Ambience/Forest");
    targetForestParam = "Cover";

    InputManager inputManager;

    forestParamsKeyedByKey['c'] = "Cover";
    forestParamsKeyedByKey['r'] = "Rain";
    forestParamsKeyedByKey['w'] = "Wind";

    bool keepLooping = true;
    while (keepLooping)
    {
        HandleInput(&inputManager, &audioManager, &keepLooping);
        audioManager.Update();
    }

    if (forestAudioController)
    {
        delete forestAudioController;
        forestAudioController = nullptr;
    }

    return 0;
}