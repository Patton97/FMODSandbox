#include "ConsoleASCIIUIPrinter.h"

#include <stdlib.h>
#include <iostream>
#include <windows.h>

static void ShowConsoleCursor(bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}

ConsoleASCIIUIPrinter::ConsoleASCIIUIPrinter()
{
    ShowConsoleCursor(false);
}

void ConsoleASCIIUIPrinter::SetString(std::string uiString)
{
    this->uiString = uiString;
    this->hasUIStringChangedSinceLastUpdate = true;
}

void ConsoleASCIIUIPrinter::Update()
{
    if (!this->hasUIStringChangedSinceLastUpdate)
        return;

    system("cls");
    std::cout << this->uiString << std::endl;

    this->hasUIStringChangedSinceLastUpdate = false;
}
