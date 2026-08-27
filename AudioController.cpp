#include "AudioController.h"

#include <conio.h>
#include <iostream>
#include <sstream>

#include "BusController.h"
#include "Utils.h"

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

void AudioController::SubscribeToSettingsChanged(OnSettingsChangedEventHandler eventHandler)
{
    this->onSettingsChangedEventHandler.push_back(eventHandler);
}

void AudioController::RaiseOnASCIIUIStringUpdated()
{
    for (int i = 0; i < this->onSettingsChangedEventHandler.size(); ++i)
    {
        this->onSettingsChangedEventHandler[i]();
    }
}

void AudioController::OnKeyPress(KeyPress keyPress)
{
    switch (keyPress.KeyCode)
    {
        case KeyCode::ArrowUp:
            ChangeControlTargetItemIndex(/*increment*/ false);
            break;

        case KeyCode::ArrowDown:
            ChangeControlTargetItemIndex(/*increment*/ true);
            break;

        case KeyCode::ArrowLeft:
            ModifyValue(/*positiveModify*/ false);
            break;

        case KeyCode::ArrowRight:
            ModifyValue(/*positiveModify*/ true);
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
    BusController* busController;
    switch (this->currentControlTarget)
    {
        case ControlTarget::Parameter:
            this->currentControlTarget = ControlTarget::Volume;
            // output volume as both feedback & a debug measure
            busController = this->audioManager.GetBusController(this->currentBusIndex);
            RaiseOnASCIIUIStringUpdated();
            break;

        case ControlTarget::Volume:
            this->currentControlTarget = ControlTarget::Parameter;
            // output value as both feedback & a debug measure
            std::string paramName = forestAudioController->GetParamName(this->currentParameterIndex);
            RaiseOnASCIIUIStringUpdated();
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

    RaiseOnASCIIUIStringUpdated();
}

void AudioController::ChangeParameterIndex(bool increment)
{
    int minIndex = 0;
    int maxIndex = this->forestAudioController->GetParamCount() - 1;
    int newIndex = this->currentParameterIndex = Utils::ModifyInt(this->currentParameterIndex, minIndex, maxIndex, increment, /*wrap*/ true);

    RaiseOnASCIIUIStringUpdated();
}

void AudioController::ModifyVolume(BusController* busController, float modifyAmount)
{
    busController->ModifyVolume(modifyAmount);

    RaiseOnASCIIUIStringUpdated();
}

void AudioController::ModifyValue(bool positiveModify)
{
    float modifyAmount = positiveModify ? this->GetValueModificationIncrement() : -this->GetValueModificationIncrement();
    switch (this->currentControlTarget)
    {
        case ControlTarget::Parameter:
            ModifyParameter(modifyAmount);
            break;

        case ControlTarget::Volume:
            ModifyVolume(this->audioManager.GetBusController(this->currentBusIndex), modifyAmount);
            break;
    }
}

void AudioController::ModifyParameter(float modifyAmount)
{
    std::string paramName = this->forestAudioController->GetParamName(this->currentParameterIndex);
    forestAudioController->ModifyParamValue(paramName, modifyAmount);

    RaiseOnASCIIUIStringUpdated();
}
