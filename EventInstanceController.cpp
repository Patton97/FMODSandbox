#include "EventInstanceController.h"

#include <algorithm>

EventInstanceController::EventInstanceController(FMOD::Studio::EventInstance* eventInstance)
{
    this->eventInstance = eventInstance;
}

EventInstanceController::~EventInstanceController()
{
    this->eventInstance->release();
    delete this->eventInstance;
    this->eventInstance == nullptr;
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
