#include <iostream>

#include "AudioController.h"
#include "AudioManager.h"
#include "AudioUIConsolePrinter.h"
#include "InputManager.h"

bool keepLooping = true;
AudioUIConsolePrinter* uiPrinter;

static void RefreshUI()
{
    uiPrinter->RefreshUI();
}

int main(int argc, char* argv[])
{
    AudioManager audioManager;
    audioManager.LoadBanksFromFolder("Assets/FMODBanks/");

    InputManager inputManager;

    AudioController audioController(inputManager, audioManager, &keepLooping);
    uiPrinter = new AudioUIConsolePrinter(audioController, audioManager);

    RefreshUI();
    audioController.SubscribeToSettingsChanged(&RefreshUI);

    while (keepLooping)
    {
        audioController.Update();
        audioManager.Update();
    }

    if (uiPrinter)
    {
        delete uiPrinter;
        uiPrinter = nullptr;
    }

    return 0;
}