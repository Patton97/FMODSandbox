#pragma once

#include "KeyPress.h"
#include "InputManager.h"
#include "AudioManager.h"

enum class ControlTarget
{
    Parameter,
    Volume,
};


class AudioController
{
    typedef void (OnSettingsChangedEventHandler)();
public:
    AudioController(InputManager& inputManager, AudioManager& audioManager, bool* keepLooping);

    void Update();
    void SubscribeToSettingsChanged(OnSettingsChangedEventHandler eventHandler);

    ControlTarget GetCurrentControlTarget() const { return this->currentControlTarget; };
    int GetCurrentBusIndex() const { return this->currentBusIndex; };
    int GetCurrentParamIndex() const { return this->currentParameterIndex; };

    int GetBusCount() const { return this->audioManager.GetBusCount(); };
    int GetParamCount() const { return this->forestAudioController->GetParamCount(); };

    float GetValueModificationIncrement() const { return this->VALUE_MODIFICATION_INCREMENT; }

    EventInstanceController* GetForestAudioController() const { return this->forestAudioController; };

private:
    const float VALUE_MODIFICATION_INCREMENT = 0.05f;

    InputManager& inputManager;
    AudioManager& audioManager;
    bool* keepLooping;

    ControlTarget currentControlTarget = ControlTarget::Volume;
    int currentBusIndex = 0;
    int currentParameterIndex = 0;

    EventInstanceController* forestAudioController;

    std::vector<OnSettingsChangedEventHandler*> onSettingsChangedEventHandler;

    void OnKeyPress(KeyPress keyPress);

    void ChangeControlTarget();
    void ChangeControlTargetItemIndex(bool increment);
    void ChangeBusIndex(bool increment);
    void ChangeParameterIndex(bool increment);
    
    void ModifyVolume(FMODBusController* busController, float modifyAmount);
    void ModifyValue(bool positiveModify);
    void ModifyParameter(float modifyAmount);

    void RaiseOnASCIIUIStringUpdated();
};

