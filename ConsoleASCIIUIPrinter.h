#pragma once

#include <string>

class ConsoleASCIIUIPrinter
{
public:
    ConsoleASCIIUIPrinter();

    void SetString(std::string uiString);
    void Update();

private:
    std::string uiString;
    bool hasUIStringChangedSinceLastUpdate = false;
};

