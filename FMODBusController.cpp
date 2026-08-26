#include "FMODBusController.h"
#include <algorithm>

FMODBusController::FMODBusController(FMOD::Studio::Bus* bus)
{
    this->bus = bus;
}

void FMODBusController::IncreaseVolume(float increaseAmount)
{
    ModifyVolume(increaseAmount);
}

void FMODBusController::DecreaseVolume(float decreaseAmount)
{
    ModifyVolume(-decreaseAmount);
}

void FMODBusController::ModifyVolume(float modifyAmount)
{
    this->SetVolume(this->GetVolume() + modifyAmount);
}

void FMODBusController::SetVolume(float newVolume)
{
    newVolume = std::clamp(newVolume, 0.0f, 1.0f);
    this->bus->setVolume(newVolume);
}

float FMODBusController::GetVolume()
{
    float volume;
    this->bus->getVolume(&volume);
    return volume;
}