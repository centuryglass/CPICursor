/**
 * @file  Main.cpp
 *
 * @brief  The main build file for the CPICursor application.
 */

#include "CursorPainter.h"
#include <iostream>
#include <unistd.h>

int main(int argc, char** argv)
{
    std::cout << "Starting cursor painter:\n";
    CursorPainter painter;
    painter.drawCursor(500,500);
    sleep(30000);
    return 0;
}
