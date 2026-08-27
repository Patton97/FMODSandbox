#include "AudioController.h"

#include <conio.h>
#include <iostream>

#include "FMODBusController.h"
#include "Utils.h"

const float VOLUME_INCREMENT = 0.05f;
const float PARAM_INCREMENT = 0.05f;

AudioController::AudioController(InputManager& inputManager, AudioManager& audioManager, bool* keepLooping)
    : inputManager(inputManager), audioManager(audioManager)
{
    this->keepLooping = keepLooping;
    forestAudioController = audioManager.PlayAudio("event:/Ambience/Forest");
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

        case KeyCode::ArrowLeft:
            ChangeControlTargetItemIndex(/*increment*/ false);
            break;

        case KeyCode::ArrowRight:
            ChangeControlTargetItemIndex(/*increment*/ true);
            break;

        case KeyCode::Escape:
            *this->keepLooping = false;
            break;

        case KeyCode::Spacebar:
            this->audioManager.PlayAudio("event:/Weapons/Explosion");
            break;

        case KeyCode::Tab:
            ChangeControlTarget();
            break;
    }
}

void AudioController::ChangeControlTarget()
{
    FMODBusController* busController;
    switch (this->currentControlTarget)
    {
        case ControlTarget::Parameter:
            this->currentControlTarget = ControlTarget::Volume;
            // output volume as both feedback & a debug measure
            busController = this->audioManager.GetBusController(this->currentBusIndex);
            Utils::PrintBusVolume(busController);
            break;

        case ControlTarget::Volume:
            this->currentControlTarget = ControlTarget::Parameter;
            // output value as both feedback & a debug measure
            std::string paramName = forestAudioController->GetParamName(this->currentParameterIndex);
            Utils::PrintParameter(forestAudioController, paramName);
            break;
    }
}

void AudioController::ChangeControlTargetItemIndex(bool increment)
{
    switch (this->currentControlTarget)
    {
        case ControlTarget::Parameter:
            ChangeParameterIndex(increment);
            break;

        case ControlTarget::Volume:
            ChangeBusIndex(increment);
            break;
    }
}

void AudioController::ChangeBusIndex(bool increment)
{
    int minIndex = 0;
    int maxIndex = this->audioManager.GetBusCount() - 1;
    int newIndex = this->currentBusIndex = Utils::ModifyInt(this->currentBusIndex, minIndex, maxIndex, increment, /*wrap*/ true);

    Utils::PrintBusVolume(this->audioManager.GetBusController(newIndex));
}

void AudioController::ChangeParameterIndex(bool increment)
{
    int minIndex = 0;
    int maxIndex = this->forestAudioController->GetParamCount() - 1;
    int newIndex = this->currentParameterIndex = Utils::ModifyInt(this->currentParameterIndex, minIndex, maxIndex, increment, /*wrap*/ true);

    std::string paramName = this->forestAudioController->GetParamName(newIndex);
    Utils::PrintParameter(this->forestAudioController, paramName);
}

void AudioController::ModifyVolume(FMODBusController* busController, float modifyAmount)
{
    busController->ModifyVolume(modifyAmount);

    // print new volume as both feedback & a debug measure
    Utils::PrintBusVolume(busController);
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
    std::string paramName = this->forestAudioController->GetParamName(this->currentParameterIndex);
    forestAudioController->ModifyParamValue(paramName, modifyAmount);

    // print new value as both feedback & a debug measure
    Utils::PrintParameter(forestAudioController, paramName);
}
