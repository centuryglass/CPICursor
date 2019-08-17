/**
 * @file  DisplayListener.h
 *
 * @brief  Listens to the cursor painter daemon to get the main display
 *         resolution.
 */

#pragma once
#include "Pipe_Listener.h"

class DisplayListener : public DaemonFramework::Pipe::Listener
{
public:
    DisplayListener() { }

    virtual ~DisplayListener() { }

    /**
     * @brief  Gets the main display's width in pixels.
     *
     * @return  The display width, or zero if the DisplayListener has not yet
     *          received display information.
     */
    size_t getDisplayWidth() const;

    /**
     * @brief  Gets the main display's height in pixels.
     *
     * @return  The display height or zero if the DisplayListener has not yet
     *          received display information.
     */
    size_t getDisplayHeight() const;

private:
    /**
     * @brief  Read the display resolution sent by the cursor painter daemon.
     *
     * @param data  A raw data pointer holding the display width and length.
     *
     * @param size  The amount of data attached to the data pointer. If this
     *              does not equal sizeof(size_t) * 2, the message will be
     *              ignored.
     */
    virtual void processData
    (const unsigned char* data, const size_t size) override;

    size_t width = 0;
    size_t height = 0;
};
