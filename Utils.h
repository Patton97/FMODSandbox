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

    static void PrintProgressBar(std::stringstream* stream, float progressValue, float incrementSize)
    {
        *stream << " [";
        for (int i = 0; i < 1 / incrementSize; ++i)
        {
            char charToPrint = i < progressValue / incrementSize ? (char)254u : ' ';
            *stream << charToPrint;
        }
        *stream << ']';
    }
}