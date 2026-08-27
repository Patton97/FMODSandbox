#pragma once

#include <fmod_studio.hpp>
#include <string>

class BusController
{
public:
    BusController(FMOD::Studio::Bus* bus);

    std::string GetPath() const;

    float GetVolume();
    void SetVolume(float newVolume);

    void IncreaseVolume(float increaseAmount = 0.1f);
    void DecreaseVolume(float decreaseAmount = 0.1f);
    void ModifyVolume(float modifyAmount = 0.1f);

private:
    FMOD::Studio::Bus* bus;
};

