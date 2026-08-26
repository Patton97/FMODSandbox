#pragma once
#include <string>
#include <fmod_studio.hpp>
class EventInstanceController
{

public:
    EventInstanceController(FMOD::Studio::EventInstance* eventInstance);
    ~EventInstanceController();

    float GetParamValue(std::string parameterName);
    void SetParamValue(std::string parameterName, float newParamValue);
    void ModifyParamValue(std::string parameterName, float modifyAmount);

private:
    FMOD::Studio::EventInstance* eventInstance;
};

