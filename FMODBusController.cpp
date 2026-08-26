#include "FMODBusController.h"
#include <algorithm>

FMODBusController::FMODBusController(FMOD::Studio::Bus* bus)
{
    this->bus = bus;
}

std::string FMODBusController::GetPath() const
{
    char path[512];
    int* retrieved = nullptr;
    this->bus->getPath(path, sizeof(path), retrieved);

    return std::string(path);
}

float FMODBusController::GetVolume()
{
    float volume;
    this->bus->getVolume(&volume);
    return volume;
}

void FMODBusController::SetVolume(float newVolume)
{
    newVolume = std::clamp(newVolume, 0.0f, 1.0f);
    this->bus->setVolume(newVolume);
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