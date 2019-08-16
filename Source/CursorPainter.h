/**
 * @file  CursorPainter.h
 *
 * @brief  Draws the mouse cursor on the screen.
 */

#pragma once
#include "DaemonControl.h"
#include <cstddef>

class CursorPainter : private DaemonFramework::DaemonControl
{
public:
    /**
     * @brief  Launches the cursor painter daemon and prepares to send it
     *         commands.
     */
    CursorPainter();

    /**
     * @brief  Ensures the cursor painter daemon is stopped on destruction.
     */
    ~CursorPainter();
    
    /**
     * @brief  Commands the cursor painter daemon to draw the cursor at a 
     *         specific coordinate.
     *
     * @param x  Screen x-coordinate, measured in pixels.
     *
     * @param y  Screen y_coordinate, measured in pixels.
     *
     * @return   Whether the CursorPainter was able to send the paint command.
     */
    bool drawCursor(const size_t x, const size_t y);
};
