#include "EventInstanceController.h"

#include <algorithm>

EventInstanceController::EventInstanceController(FMOD::Studio::EventInstance* eventInstance)
{
    this->eventInstance = eventInstance;

    FMOD::Studio::EventDescription* desc;
    this->eventInstance->getDescription(&desc);

    int paramCount;
    desc->getParameterDescriptionCount(&paramCount);
    for (int i = 0; i <= paramCount; ++i)
    {
        FMOD_STUDIO_PARAMETER_DESCRIPTION paramDesc;
        FMOD_RESULT getParamResult = desc->getParameterDescriptionByIndex(i, &paramDesc);
        if (getParamResult == FMOD_OK)
            this->paramDescriptions.push_back(paramDesc);
    }
}

EventInstanceController::~EventInstanceController()
{
    this->eventInstance->release();
    delete this->eventInstance;
    this->eventInstance == nullptr;
}

std::string EventInstanceController::GetName()
{
    FMOD::Studio::EventDescription* desc;
    this->eventInstance->getDescription(&desc);

    char path[512];
    int* retrieved = nullptr;
    desc->getPath(path, sizeof(path), retrieved);

    return std::string(path);
}

std::string EventInstanceController::GetParamName(int index)
{
    return std::string(this->paramDescriptions[index].name);
}

int EventInstanceController::GetParamCount()
{
    return this->paramDescriptions.size();
}

float EventInstanceController::GetParamValue(std::string parameterName)
{
    float currentParamValue = 0.0f;
    this->eventInstance->getParameterByName(parameterName.c_str(), &currentParamValue);
    return currentParamValue;
}

void EventInstanceController::SetParamValue(std::string parameterName, float newParamValue)
{
    newParamValue = std::clamp(newParamValue, 0.0f, 1.0f);
    this->eventInstance->setParameterByName(parameterName.c_str(), newParamValue);
}

void EventInstanceController::ModifyParamValue(std::string parameterName, float modifyAmount)
{
    float newParamValue = this->GetParamValue(parameterName) + modifyAmount;
    this->SetParamValue(parameterName, newParamValue);
}
