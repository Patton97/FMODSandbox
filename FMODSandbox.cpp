#include "AudioController.h"
#include "AudioManager.h"
#include "InputManager.h"

bool keepLooping = true;

int main(int argc, char* argv[])
{
    AudioManager audioManager;
    audioManager.LoadBanksFromFolder("Assets/FMODBanks/");

    InputManager inputManager;

    AudioController audioController(inputManager, audioManager, &keepLooping);

    while (keepLooping)
    {
        audioController.Update();
        audioManager.Update();
    }

    return 0;
}