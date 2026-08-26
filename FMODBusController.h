#pragma once

#include <fmod_studio.hpp>

class FMODBusController
{
public:
    FMODBusController(FMOD::Studio::Bus* bus);

    void IncreaseVolume(float increaseAmount = 0.1f);
    void DecreaseVolume(float decreaseAmount = 0.1f);
    void ModifyVolume(float modifyAmount = 0.1f);

    float GetVolume();
    void SetVolume(float newVolume);

private:
    FMOD::Studio::Bus* bus;
};

