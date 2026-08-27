#include "AudioController.h"

#include <conio.h>
#include <iostream>
#include <sstream>

#include "FMODBusController.h"
#include "Utils.h"

const float MODIFY_VALUE_INCREMENT = 0.05f;

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

void AudioController::SubscribeToASCIIUIStringUpdates(OnASCIIUIStringUpdatedEventHandler eventHandler)
{
    this->onASCIIUIStringUpdatedEventHandlers.push_back(eventHandler);
}

std::string AudioController::GetASCIIUIString()
{
    std::stringstream ss;

    std::string selectedIndicator = ">> ";
    std::string unselectedIndicator = "   ";

    for (int i = 0; i < this->audioManager.GetBusCount(); ++i)
    {
        FMODBusController* busController = this->audioManager.GetBusController(i);
        bool isSelected = this->currentControlTarget == ControlTarget::Volume && this->currentBusIndex == i;
        ss << (isSelected ? selectedIndicator : unselectedIndicator);

        std::string busPath = busController->GetPath();
        ss << busPath;

        // this should really be determined from bus list, but for now this works
        constexpr int MAX_BUS_PATH_LENGTH = 19;
        if (busPath.size() < MAX_BUS_PATH_LENGTH)
            ss << std::string(MAX_BUS_PATH_LENGTH - busPath.size(), ' ');

        std::string pctString = std::to_string(Utils::FloatToIntPct(busController->GetVolume()));
        ss << " | Volume: " << pctString << "%";

        constexpr int MAX_PCT_STR_LENGTH = 3;
        if (pctString.size() < MAX_PCT_STR_LENGTH)
            ss << std::string(MAX_PCT_STR_LENGTH - pctString.size(), ' ');

        Utils::PrintProgressBar(&ss, busController->GetVolume(), MODIFY_VALUE_INCREMENT);
        ss << std::endl;
    }

    ss << std::string(65, '=') << std::endl;

    EventInstanceController* audioController = this->forestAudioController;
    for (int i = 0; i < audioController->GetParamCount(); ++i)
    {
        bool isSelected = this->currentControlTarget == ControlTarget::Parameter && this->currentParameterIndex == i;
        std::string audioName = audioController->GetName();
        std::string paramName = audioController->GetParamName(i);
        float paramValue = audioController->GetParamValue(paramName);
        std::string pctString = std::to_string(Utils::FloatToIntPct(paramValue));

        ss << (isSelected ? selectedIndicator : unselectedIndicator);
        ss << audioName << " | " << paramName;

        // this should really be determined from param list, but for now this works
        constexpr int MAX_PARAM_NAME_LENGTH = 5;
        if (pctString.size() < MAX_PARAM_NAME_LENGTH)
            ss << std::string(MAX_PARAM_NAME_LENGTH - paramName.size(), ' ');

        ss << ": " << pctString << "%";;

        constexpr int MAX_PCT_STR_LENGTH = 3;
        if (pctString.size() < MAX_PCT_STR_LENGTH)
            ss << std::string(MAX_PCT_STR_LENGTH - pctString.size(), ' ');

        Utils::PrintProgressBar(&ss, paramValue, MODIFY_VALUE_INCREMENT);
        ss << std::endl;
    }

    return ss.str();
}

void AudioController::RaiseOnASCIIUIStringUpdated()
{
    std::string asciiUIString = this->GetASCIIUIString();
    for (int i = 0; i < this->onASCIIUIStringUpdatedEventHandlers.size(); ++i)
    {
        this->onASCIIUIStringUpdatedEventHandlers[i](asciiUIString);
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
    FMODBusController* busController;
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

void AudioController::ModifyVolume(FMODBusController* busController, float modifyAmount)
{
    busController->ModifyVolume(modifyAmount);

    RaiseOnASCIIUIStringUpdated();
}

void AudioController::ModifyValue(bool positiveModify)
{
    switch (this->currentControlTarget)
    {
        case ControlTarget::Parameter:
            ModifyParameter(positiveModify ? MODIFY_VALUE_INCREMENT : -MODIFY_VALUE_INCREMENT);
            break;

        case ControlTarget::Volume:
            ModifyVolume(this->audioManager.GetBusController(this->currentBusIndex), positiveModify ? MODIFY_VALUE_INCREMENT : -MODIFY_VALUE_INCREMENT);
            break;
    }
}

void AudioController::ModifyParameter(float modifyAmount)
{
    std::string paramName = this->forestAudioController->GetParamName(this->currentParameterIndex);
    forestAudioController->ModifyParamValue(paramName, modifyAmount);

    RaiseOnASCIIUIStringUpdated();
}
