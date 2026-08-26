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
FMOD::Studio::EventInstance* forestAudio;
std::map<char, std::string> forestParamsKeyedByKey;
std::string targetForestParam;
ControlTarget currentControlTarget = ControlTarget::Volume;

static int FloatToIntPct(float f)
{
    float rounded = std::round(f * 100.0) / 100.0;
    return (int)(rounded * 100);
}

static void ModifyVolume(FMODBusController* busController, float modifyAmount)
{
    busController->ModifyVolume(modifyAmount);

    // output new volume as both feedback & a debug measure
    std::cout << busController->GetPath() << " | Volume = " << busController->GetVolume() << std::endl;
}

static void ModifyParameter(float modifyAmount)
{
    float currentParamValue = 0.0f;
    forestAudio->getParameterByName(targetForestParam.c_str(), &currentParamValue);

    float newParamValue = std::clamp(currentParamValue + modifyAmount, 0.0f, 1.0f);
    forestAudio->setParameterByName(targetForestParam.c_str(), newParamValue);

    std::cout << "Forest audio parameter '" << targetForestParam << "': " << FloatToIntPct(newParamValue) << "%" << std::endl;
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

static void handleInput(InputManager* inputManager, AudioManager* audioManager, bool* keepLooping)
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

                    float currentParamValue = 0.0f;
                    forestAudio->getParameterByName(targetForestParam.c_str(), &currentParamValue);
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
    forestAudio = audioManager.PlayAudio("event:/Ambience/Forest");
    targetForestParam = "Cover";

    InputManager inputManager;

    forestParamsKeyedByKey['c'] = "Cover";
    forestParamsKeyedByKey['r'] = "Rain";
    forestParamsKeyedByKey['w'] = "Wind";

    bool keepLooping = true;
    while (keepLooping)
    {
        handleInput(&inputManager, &audioManager, &keepLooping);
        audioManager.Update();
    }

    if (forestAudio)
    {
        delete forestAudio;
        forestAudio = nullptr;
    }

    return 0;
}