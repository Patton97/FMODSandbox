#pragma once

#include <string>
#include <fmod_studio.hpp>
#include <vector>

class EventInstanceController
{

public:
    EventInstanceController(FMOD::Studio::EventInstance* eventInstance);
    ~EventInstanceController();

    std::string GetName();

    std::string GetParamName(int index);
    int GetParamCount();

    float GetParamValue(std::string parameterName);
    void SetParamValue(std::string parameterName, float newParamValue);
    void ModifyParamValue(std::string parameterName, float modifyAmount);

private:
    FMOD::Studio::EventInstance* eventInstance;
    std::vector<FMOD_STUDIO_PARAMETER_DESCRIPTION> paramDescriptions;
};

