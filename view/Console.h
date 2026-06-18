#ifndef ILMS_CONSOLE_H
#define ILMS_CONSOLE_H

#include <string>

// Wraps every Windows-console / OS specific call the program makes.
// All methods are static because the console is a single shared resource.
class Console
{
public:
    // Move the text cursor to column x, row y.
    static void gotoxy(int x, int y);

    // Set the console text colour attribute.
    static void setColor(int color);

    // Show (true) or hide (false) the blinking cursor.
    static void cursor(bool visible);

    // Configure buffer size and window dimensions; hides the cursor.
    static void screenSetup(int bufferX, int bufferY, int width, int height);

    // Blank out the rectangle (startX,startY)..(endX,endY) with spaces.
    static bool clear(int startX, int startY, int endX, int endY);

    // Current date  as "YYYY-MM-DD".
    static std::string date();

    // Current time as "HH:MM:SS".
    static std::string time();
};

#endif // ILMS_CONSOLE_H
