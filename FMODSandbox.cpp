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
FMOD::Studio::EventInstance* forestAudio;

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

        case KeyCode::Character:
            FMOD_RESULT result;
            switch (keyPress.Character)
            {
                case 'c':
                    result = forestAudio->setParameterByName("Cover", 0.5f);
                    break;

                case 'r':
                    result = forestAudio->setParameterByName("Rain", 0.5f);
                    break;

                case 'w':
                    result = forestAudio->setParameterByName("Wind", 0.5f);
                    break;
            }
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
    forestAudio = audioManager.PlayAudio("event:/Ambience/Forest");

    InputManager inputManager;

    bool keepLooping = true;
    while (keepLooping)
    {
        handleInput(&inputManager, &audioManager, &keepLooping);
        audioManager.Update();
    }

    if (forestAudio)
    {
        delete forestAudio;
        forestAudio = nullptr;
    }

    return 0;
}