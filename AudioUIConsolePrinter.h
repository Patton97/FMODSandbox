#pragma once

#include <string>
#include "AudioController.h"

class AudioUIConsolePrinter
{
public:
    AudioUIConsolePrinter(AudioController& audioController, AudioManager& audioManager);

    void RefreshUI() const;

private:
    AudioController& audioController;
    AudioManager& audioManager;

    std::string selectedIndicator = ">> ";
    std::string unselectedIndicator = "   ";

    std::string GetASCIIUIString() const;
    void PrintSelectedIndicator(std::stringstream* stream, bool isSelected) const;
};

