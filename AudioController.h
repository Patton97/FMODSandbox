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
public:
    AudioController(InputManager& inputManager, AudioManager& audioManager, bool* keepLooping);

    void Update();

private:
    InputManager& inputManager;
    AudioManager& audioManager;
    bool* keepLooping;

    ControlTarget currentControlTarget = ControlTarget::Volume;
    int currentBusIndex = 0;

    EventInstanceController* forestAudioController;
    std::map<char, std::string> forestParamsKeyedByKey;
    std::string targetForestParam = "Cover";

    void OnKeyPress(KeyPress keyPress);
    void ModifyValue(bool positiveModify);
    void ModifyParameter(float modifyAmount);
};

