/**
 * @file  CursorPainter.h
 *
 * @brief  Draws the mouse cursor on the screen.
 */

#pragma once
#include "DaemonControl.h"
#include "DisplayListener.h"
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

    /**
     * @brief  Gets the main display's width in pixels.
     *
     * @return  The display width, or zero if the CursorPainter has not yet
     *          received display information.
     */
    size_t getDisplayWidth() const;

    /**
     * @brief  Gets the main display's height in pixels.
     *
     * @return  The display height, or zero if the CursorPainter has not yet
     *          received display information.
     */
    size_t getDisplayHeight() const;

private:
    // Receives display resolution sent by the painter daemon.
    DisplayListener listener;
};
