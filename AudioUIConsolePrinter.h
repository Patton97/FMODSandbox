#pragma once

#include <string>
#include "AudioController.h"

class AudioUIConsolePrinter
{
public:
    AudioUIConsolePrinter(AudioController& audioController, AudioManager& audioManager);

    void RefreshUI();

private:
    AudioController& audioController;
    AudioManager& audioManager;

    std::string GetASCIIUIString() const;
};

