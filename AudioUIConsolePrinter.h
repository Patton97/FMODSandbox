#pragma once

#include <string>
#include "AudioController.h"

class AudioUIConsolePrinter
{
public:
    AudioUIConsolePrinter(AudioController& audioController, AudioManager& audioManager);

    void Update();
    void RefreshUI();

private:
    std::string uiString;
    bool hasUIStringChangedSinceLastUpdate = false;
    AudioController& audioController;
    AudioManager& audioManager;

    std::string GetASCIIUIString() const;
};

