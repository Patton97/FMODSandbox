#pragma once

#include <string>

#include "FMODBusController.h"
#include "EventInstanceController.h"

namespace Utils
{
    static int FloatToIntPct(float f)
    {
        float rounded = std::round(f * 100.0) / 100.0;
        return (int)(rounded * 100);
    }

    static void PrintBusVolume(FMODBusController* busController)
    {
        // output new volume as both feedback & a debug measure
        std::cout << busController->GetPath() << " | Volume = " << busController->GetVolume() << std::endl;
    }

    static void PrintParameter(EventInstanceController* eventInstanceController, std::string parameterName)
    {
        float paramValue = eventInstanceController->GetParamValue(parameterName);
        std::cout << eventInstanceController->GetName() << " | " << parameterName << " = " << FloatToIntPct(paramValue) << "%" << std::endl;
    }

    static int ModifyInt(int currentValue, int minValue, int maxValue, bool increment, bool wrap)
    {
        int newIndex = increment ? currentValue + 1 : currentValue - 1;

        if (newIndex > maxValue)
            return wrap ? minValue : maxValue;
        else if (newIndex < minValue)
            return wrap ? maxValue : minValue;
        else
            return newIndex;
    }
}