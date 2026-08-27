#include "AudioUIConsolePrinter.h"

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <windows.h>

#include "Utils.h"

typedef std::chrono::high_resolution_clock Clock;

static void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

static void PrintProgressBar(std::stringstream* stream, float progressValue, float incrementSize)
{
    *stream << " [";
    for (int i = 0; i < 1 / incrementSize; ++i)
    {
        char charToPrint = i < progressValue / incrementSize ? (char)254u : ' ';
        *stream << charToPrint;
    }
    *stream << ']';
}

static void PrintPercentage(std::stringstream* stream, int pctValue)
{
    std::string pctString = std::to_string(pctValue);

    constexpr int MAX_PCT_STR_LENGTH = 3;
    if (pctString.size() < MAX_PCT_STR_LENGTH)
        *stream << std::string(MAX_PCT_STR_LENGTH - pctString.size(), ' ');

    *stream << pctString << "%";
}

AudioUIConsolePrinter::AudioUIConsolePrinter(
    AudioController& audioController,
    AudioManager& audioManager)
    : audioController(audioController), audioManager(audioManager)
{
    ShowConsoleCursor(false);
}

void AudioUIConsolePrinter::RefreshUI() const
{
    std::string uiString = this->GetASCIIUIString();
    system("cls");
    std::cout << uiString << std::endl;
}

std::string AudioUIConsolePrinter::GetASCIIUIString() const
{
    std::stringstream stream;

    auto uiStringBuildStartTime = Clock::now();
    this->BuildBusInfoString(&stream);

    stream << std::string(65, '=') << std::endl;

    this->BuildParameterInfoString(&stream);

    auto uiStringBuildEndTime = Clock::now();
    this->BuildUIPerformanceMetricsString(&stream, uiStringBuildStartTime, uiStringBuildEndTime);

    return stream.str();
}

std::string AudioUIConsolePrinter::GetSelectedIndicator(std::stringstream* stream, bool isSelected) const
{
    return isSelected ? this->selectedIndicator : this->unselectedIndicator;
}

void AudioUIConsolePrinter::BuildBusInfoString(std::stringstream* stream) const
{
    ControlTarget currentControlTarget = this->audioController.GetCurrentControlTarget();

    for (int i = 0; i < this->audioController.GetBusCount(); ++i)
    {
        BusController* busController = this->audioManager.GetBusController(i);
        bool isSelected = currentControlTarget == ControlTarget::Volume && this->audioController.GetCurrentBusIndex() == i;
        *stream << (isSelected ? this->selectedIndicator : this->unselectedIndicator);

        std::string busPath = busController->GetPath();
        *stream << busPath;

        // this should really be determined from bus list, but for now this works
        constexpr int MAX_BUS_PATH_LENGTH = 19;
        if (busPath.size() < MAX_BUS_PATH_LENGTH)
            *stream << std::string(MAX_BUS_PATH_LENGTH - busPath.size(), ' ');

        *stream << " | Volume: ";

        PrintPercentage(stream, Utils::FloatToIntPct(busController->GetVolume()));
        PrintProgressBar(stream, busController->GetVolume(), this->audioController.GetValueModificationIncrement());

        *stream << std::endl;
    }
}

void AudioUIConsolePrinter::BuildParameterInfoString(std::stringstream* stream) const
{
    ControlTarget currentControlTarget = this->audioController.GetCurrentControlTarget();

    EventInstanceController* eventInstanceController = this->audioController.GetForestAudioController();
    for (int i = 0; i < eventInstanceController->GetParamCount(); ++i)
    {
        bool isSelected = currentControlTarget == ControlTarget::Parameter && this->audioController.GetCurrentParamIndex() == i;

        std::string audioName = eventInstanceController->GetName();
        std::string paramName = eventInstanceController->GetParamName(i);
        float paramValue = eventInstanceController->GetParamValue(paramName);

        *stream << (isSelected ? selectedIndicator : unselectedIndicator);
        *stream << audioName << " | " << paramName;

        // this should really be determined from param list, but for now this works
        constexpr int MAX_PARAM_NAME_LENGTH = 5;
        if (paramName.size() < MAX_PARAM_NAME_LENGTH)
            *stream << std::string(MAX_PARAM_NAME_LENGTH - paramName.size(), ' ');

        *stream << ": ";

        PrintPercentage(stream, Utils::FloatToIntPct(paramValue));
        PrintProgressBar(stream, paramValue, this->audioController.GetValueModificationIncrement());
        *stream << std::endl;
    }
}

void AudioUIConsolePrinter::BuildUIPerformanceMetricsString(
    std::stringstream* stream,
    std::chrono::steady_clock::time_point& uiStringBuildStartTime,
    std::chrono::steady_clock::time_point& uiStringBuildEndTime) const
{
    auto uiStringBuildTimeElapsed = uiStringBuildEndTime - uiStringBuildStartTime;
    float uiStringBuildTimeElapsedMs = uiStringBuildTimeElapsed.count() / 1000000.0f;
    *stream << std::endl << "UI build time: " << uiStringBuildTimeElapsedMs << "ms" << std::endl;
}
