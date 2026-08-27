#pragma once

#include <chrono>
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
    std::string GetSelectedIndicator(std::stringstream* stream, bool isSelected) const;
    void BuildBusInfoString(std::stringstream* stream) const;
    void BuildParameterInfoString(std::stringstream* stream) const;
    void BuildUIPerformanceMetricsString(
        std::stringstream* stream,
        std::chrono::steady_clock::time_point& uiStringBuildStartTime,
        std::chrono::steady_clock::time_point& uiStringBuildEndTime
    ) const;
};

