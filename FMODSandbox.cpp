#include <iostream>

#include "AudioController.h"
#include "AudioManager.h"
#include "ConsoleASCIIUIPrinter.h"
#include "InputManager.h"

bool keepLooping = true;
ConsoleASCIIUIPrinter* uiPrinter;

static void SetPrinterString(std::string string)
{
    uiPrinter->SetString(string);
}

int main(int argc, char* argv[])
{
    AudioManager audioManager;
    audioManager.LoadBanksFromFolder("Assets/FMODBanks/");

    InputManager inputManager;

    AudioController audioController(inputManager, audioManager, &keepLooping);
    uiPrinter = new ConsoleASCIIUIPrinter();

    audioController.SubscribeToASCIIUIStringUpdates(&SetPrinterString);

    while (keepLooping)
    {
        audioController.Update();
        audioManager.Update();
        uiPrinter->Update();
    }

    if (uiPrinter)
    {
        delete uiPrinter;
        uiPrinter = nullptr;
    }

    return 0;
}