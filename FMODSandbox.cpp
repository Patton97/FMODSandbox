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

void handleInput(InputManager* inputManager, AudioManager* audioManager, bool* keepLooping)
{
    if (!_kbhit())
        return;

    KeyPress keyPress = inputManager->GetKeyPress();
    switch (keyPress.KeyCode)
    {
        case KeyCode::Unknown:
            std::cout << "Unknown key pressed!" << std::endl;
            break;

        case KeyCode::Spacebar:
            audioManager->PlayAudio("event:/Weapons/Explosion");
            break;

        case KeyCode::Escape:
            *keepLooping = false;
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