#include "AudioController.h"

#include <conio.h>
#include <iostream>

#include "FMODBusController.h"

const float VOLUME_INCREMENT = 0.05f;
const float PARAM_INCREMENT = 0.05f;

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

AudioController::AudioController(InputManager& inputManager, AudioManager& audioManager, bool* keepLooping)
    : inputManager(inputManager), audioManager(audioManager)
{
    this->keepLooping = keepLooping;
    forestAudioController = audioManager.PlayAudio("event:/Ambience/Forest");

    forestParamsKeyedByKey['c'] = "Cover";
    forestParamsKeyedByKey['r'] = "Rain";
    forestParamsKeyedByKey['w'] = "Wind";
}

void AudioController::Update()
{
    if (!_kbhit())
        return;

    KeyPress keyPress = this->inputManager.GetKeyPress();
    this->OnKeyPress(keyPress);
}

void AudioController::OnKeyPress(KeyPress keyPress)
{
    switch (keyPress.KeyCode)
    {
        case KeyCode::Unknown:
            std::cout << "Unknown key pressed!" << std::endl;
            break;

        case KeyCode::ArrowUp:
            ModifyValue(/*positiveModify*/ true);
            break;

        case KeyCode::ArrowDown:
            ModifyValue(/*positiveModify*/ false);
            break;

        case KeyCode::Character:
            FMODBusController* busController;
            switch (keyPress.Character)
            {
            case 'v':
                this->currentControlTarget = ControlTarget::Volume;
                // output new volume as both feedback & a debug measure
                busController = this->audioManager.GetBusController(this->currentBusIndex);
                PrintBusVolume(busController);
                break;

            default:
                this->currentControlTarget = ControlTarget::Parameter;
                targetForestParam = forestParamsKeyedByKey[keyPress.Character];

                float currentParamValue = forestAudioController->GetParamValue(targetForestParam);
                PrintForestAudioParameterValue(targetForestParam, currentParamValue);
                break;
            }
            break;

        case KeyCode::Escape:
            *this->keepLooping = false;
            break;

        case KeyCode::Spacebar:
            this->audioManager.PlayAudio("event:/Weapons/Explosion");
            break;

        case KeyCode::Tab:
            this->currentBusIndex++;
            if (this->currentBusIndex >= this->audioManager.GetBusCount())
            {
                this->currentBusIndex = 0;
            }

            std::cout << "Selected bus: " << this->audioManager.GetBusController(this->currentBusIndex)->GetPath() << std::endl;
            break;
    }
}

void AudioController::ModifyValue(bool positiveModify)
{
    switch (this->currentControlTarget)
    {
        case ControlTarget::Parameter:
            ModifyParameter(positiveModify ? PARAM_INCREMENT : -PARAM_INCREMENT);
            break;

        case ControlTarget::Volume:
            ModifyVolume(this->audioManager.GetBusController(this->currentBusIndex), positiveModify ? VOLUME_INCREMENT : -VOLUME_INCREMENT);
            break;
    }
}

void AudioController::ModifyParameter(float modifyAmount)
{
    forestAudioController->ModifyParamValue(targetForestParam, modifyAmount);

    // print new value as both feedback & a debug measure
    PrintForestAudioParameterValue(targetForestParam, forestAudioController->GetParamValue(targetForestParam));
}

