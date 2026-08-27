#include "AudioUIConsolePrinter.h"

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <windows.h>

#include "Utils.h"

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

AudioUIConsolePrinter::AudioUIConsolePrinter(
    AudioController& audioController,
    AudioManager& audioManager)
    : audioController(audioController), audioManager(audioManager)
{
    ShowConsoleCursor(false);
}

void AudioUIConsolePrinter::RefreshUI()
{
    this->uiString = this->GetASCIIUIString();
    this->hasUIStringChangedSinceLastUpdate = true;
}

void AudioUIConsolePrinter::Update()
{
    if (!this->hasUIStringChangedSinceLastUpdate)
        return;

    this->hasUIStringChangedSinceLastUpdate = false;
    system("cls");
    std::cout << this->uiString << std::endl;
}

std::string AudioUIConsolePrinter::GetASCIIUIString() const
{
    std::stringstream ss;

    std::string selectedIndicator = ">> ";
    std::string unselectedIndicator = "   ";

    ControlTarget currentControlTarget = this->audioController.GetCurrentControlTarget();

    for (int i = 0; i < this->audioController.GetBusCount(); ++i)
    {
        BusController* busController = this->audioManager.GetBusController(i);
        bool isSelected = currentControlTarget == ControlTarget::Volume && this->audioController.GetCurrentBusIndex() == i;
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

        PrintProgressBar(&ss, busController->GetVolume(), this->audioController.GetValueModificationIncrement());
        ss << std::endl;
    }

    ss << std::string(65, '=') << std::endl;

    EventInstanceController* eventInstanceController = this->audioController.GetForestAudioController();
    for (int i = 0; i < eventInstanceController->GetParamCount(); ++i)
    {
        bool isSelected = currentControlTarget == ControlTarget::Parameter && this->audioController.GetCurrentParamIndex() == i;
        std::string audioName = eventInstanceController->GetName();
        std::string paramName = eventInstanceController->GetParamName(i);
        float paramValue = eventInstanceController->GetParamValue(paramName);
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

        PrintProgressBar(&ss, paramValue, this->audioController.GetValueModificationIncrement());
        ss << std::endl;
    }

    return ss.str();
}
