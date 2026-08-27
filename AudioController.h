#pragma once
#include "KeyPress.h"
#include "InputManager.h"
#include "AudioManager.h"

enum class ControlTarget
{
    Parameter,
    Volume,
};

typedef void (OnASCIIUIStringUpdatedEventHandler)(std::string);

class AudioController
{
public:
    AudioController(InputManager& inputManager, AudioManager& audioManager, bool* keepLooping);

    void Update();
    void SubscribeToASCIIUIStringUpdates(OnASCIIUIStringUpdatedEventHandler eventHandler);

private:
    InputManager& inputManager;
    AudioManager& audioManager;
    bool* keepLooping;

    ControlTarget currentControlTarget = ControlTarget::Volume;
    int currentBusIndex = 0;
    int currentParameterIndex = 0;

    EventInstanceController* forestAudioController;

    std::vector<OnASCIIUIStringUpdatedEventHandler*> onASCIIUIStringUpdatedEventHandlers;

    void OnKeyPress(KeyPress keyPress);

    void ChangeControlTarget();
    void ChangeControlTargetItemIndex(bool increment);
    void ChangeBusIndex(bool increment);
    void ChangeParameterIndex(bool increment);
    
    void ModifyVolume(FMODBusController* busController, float modifyAmount);
    void ModifyValue(bool positiveModify);
    void ModifyParameter(float modifyAmount);

    std::string GetASCIIUIString();
    void RaiseOnASCIIUIStringUpdated();
};

