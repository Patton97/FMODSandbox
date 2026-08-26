#include <chrono>
#include <conio.h>
#include <iostream>
#include <map>
#include <thread>

#include <fmod.hpp>
#include <fmod_errors.h>
#include <fmod_studio.hpp>

#include "AudioManager.h"
#include "InputManager.h"

int busControllerIndex = 0;
const float VOLUME_INCREMENT = 0.1f;

static void ModifyVolume(FMODBusController* busController, float modifyAmount)
{
    busController->ModifyVolume(modifyAmount);

    // output new volume as both feedback & a debug measure
    std::cout << busController->GetPath() << " | Volume = " << busController->GetVolume() << std::endl;
}

static void handleInput(InputManager* inputManager, AudioManager* audioManager, bool* keepLooping)
{
    if (!_kbhit())
        return;

    KeyPress keyPress = inputManager->GetKeyPress();
    switch (keyPress.KeyCode)
    {
        case KeyCode::Unknown:
            std::cout << "Unknown key pressed!" << std::endl;
            break;

        case KeyCode::ArrowUp:
            ModifyVolume(audioManager->GetBusController(busControllerIndex), VOLUME_INCREMENT);
            break;

        case KeyCode::ArrowDown:
            ModifyVolume(audioManager->GetBusController(busControllerIndex), -VOLUME_INCREMENT);
            break;

        case KeyCode::Escape:
            *keepLooping = false;
            break;

        case KeyCode::Spacebar:
            audioManager->PlayAudio("event:/Weapons/Explosion");
            break;

        case KeyCode::Tab:
            busControllerIndex++;
            if (busControllerIndex >= audioManager->GetBusCount())
            {
                busControllerIndex = 0;
            }

            std::cout << "Selected bus: " << audioManager->GetBusController(busControllerIndex)->GetPath() << std::endl;
            break;
    }
}

int main(int argc, char* argv[])
{
    AudioManager audioManager;
    audioManager.LoadBanksFromFolder("Assets/FMODBanks/");
    audioManager.PlayAudio("event:/Ambience/Forest");

    InputManager inputManager;

    bool keepLooping = true;
    while (keepLooping)
    {
        handleInput(&inputManager, &audioManager, &keepLooping);
        audioManager.Update();
    }

    return 0;
}