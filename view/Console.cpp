#include "Console.h"

#include <windows.h>
#include <ctime>
#include <iostream>

void Console::gotoxy(int x, int y)
{
    COORD coords;
    coords.X = static_cast<SHORT>(x);
    coords.Y = static_cast<SHORT>(y);
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coords);
}

void Console::setColor(int color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(color));
}

void Console::cursor(bool visible)
{
    CONSOLE_CURSOR_INFO curInfo;
    GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
    curInfo.bVisible = visible;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curInfo);
}

void Console::screenSetup(int bufferX, int bufferY, int width, int height)
{
    COORD bufferSize;
    bufferSize.X = static_cast<SHORT>(bufferX);
    bufferSize.Y = static_cast<SHORT>(bufferY);

    SMALL_RECT windowSize;
    windowSize.Left = 0;
    windowSize.Top = 0;
    windowSize.Right = static_cast<SHORT>(width - 1);
    windowSize.Bottom = static_cast<SHORT>(height - 1);

    SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), bufferSize);
    SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &windowSize);

    RECT rect;
    GetWindowRect(GetConsoleWindow(), &rect);
    int winWidth = rect.right - rect.left;
    int winHeight = rect.bottom - rect.top;

    SetWindowPos(GetConsoleWindow(), 0, 250, 100, winWidth, winHeight, SWP_NOZORDER | SWP_NOACTIVATE);
    cursor(false);
}

bool Console::clear(int startX, int startY, int endX, int endY)
{
    for (int i = startY; i <= endY; i++)
    {
        gotoxy(startX, i);
        for (int j = startX; j <= endX; j++)
            std::cout << " ";
    }
    return true;
}

std::string Console::date()
{
    time_t now = std::time(0);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", localtime(&now));
    return buffer;
}

std::string Console::time()
{
    time_t now = std::time(0);
    char buffer[11];
    strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&now));
    return buffer;
}
