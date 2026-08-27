#include "BusController.h"
#include <algorithm>

BusController::BusController(FMOD::Studio::Bus* bus)
{
    this->bus = bus;
}

std::string BusController::GetPath() const
{
    char path[512];
    int* retrieved = nullptr;
    this->bus->getPath(path, sizeof(path), retrieved);

    return std::string(path);
}

float BusController::GetVolume()
{
    float volume;
    this->bus->getVolume(&volume);
    return volume;
}

void BusController::SetVolume(float newVolume)
{
    newVolume = std::clamp(newVolume, 0.0f, 1.0f);
    this->bus->setVolume(newVolume);
}

void BusController::IncreaseVolume(float increaseAmount)
{
    ModifyVolume(increaseAmount);
}

void BusController::DecreaseVolume(float decreaseAmount)
{
    ModifyVolume(-decreaseAmount);
}

void BusController::ModifyVolume(float modifyAmount)
{
    this->SetVolume(this->GetVolume() + modifyAmount);
}