/**
 * @file  cursorPainterd/Source/Main.cpp
 *
 * @brief  The main build file for the daemon that draws the cursor.
 */

#include "PainterLoop.h"

int main(int argc, char** argv)
{
    PainterLoop painterLoop;
    return painterLoop.runLoop();
}
