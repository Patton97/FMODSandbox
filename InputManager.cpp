#include "InputManager.h"
#include <conio.h>

KeyPress InputManager::GetKeyPress()
{
    int key = _getch();

    if (key == 0 || key == 224)
    {
        key = _getch();

        switch (key)
        {
            case 72: return { KeyCode::ArrowUp, 0 };
            case 80: return { KeyCode::ArrowDown, 0 };
            default: return { KeyCode::Unknown, 0 };
        }
    }

    switch (key)
    {
        case  9: return { KeyCode::Tab, 0 };
        case 27: return { KeyCode::Escape, 0 };
        case 32: return { KeyCode::Spacebar, 0 };
        default: return { KeyCode::Character, (char)key };
    }
}