/**
 * @file  PainterLoop.h
 *
 * @brief  The main daemon action loop responsible for drawing to the frame
 *         buffer.
 */

#pragma once
#include "DaemonLoop.h"
#include "ImagePainter.h"
#include "FrameBuffer.h"
#include <mutex>
#include <cstddef>
#include <chrono>

class PainterLoop : public DaemonFramework::DaemonLoop
{
public:
    /**
     * @brief  Initializes cursor image data on construction, and sends the
     *         display resolution back to CPICursor.
     */
    PainterLoop();

    virtual ~PainterLoop() { }

private:
    /**
     * @brief  Periodically checks for pending cursor drawing commands,
     *         redrawing the cursor no more than once per loop.
     *
     * @return  Zero, in order to keep the loop running until the daemon is
     *          terminated.
     */
    virtual int loopAction() final override;

    /**
     * @brief  Reads cursor drawing coordinates sent from CPICursor, and
     *         buffers them until loopAction can handle them.
     *
     * @param messageData  Message data, which should consist of two size_t
     *                     values representing display pixel coordinates.
     *
     * @param messageSize  The size of the message. If this does not equal
     *                     2 * sizeof(size_t), the message is invalid.
     */
    virtual void handleParentMessage(const unsigned char* messageData,
            const size_t messageSize) override;

    /**
     * @brief  Stores a screen coordinate in pixels.
     */
    struct DrawPoint
    {
        size_t x;
        size_t y;
    };
    // Last drawn point:
    DrawPoint lastDrawn;
    // Last draw time:
    std::chrono::time_point<std::chrono::high_resolution_clock,
            std::chrono::nanoseconds> lastDrawTime;

    // Managing pending cursor draw commands:
    // Whether the first draw command has been sent:
    bool gotFirstMessage = false;
    // Maximum number of draw positions to buffer:
    static const constexpr int pointBufSize = 10;
    // Queue of pending cursor draw points:
    DrawPoint pointBuffer [pointBufSize] = {0};
    // Number of points currently buffered:
    size_t bufferedPointCount = 0;
    // Sliding index tracking the front of the buffer queue:
    size_t startIndex = 0;
    // Prevents simultaneous buffer updates:
    std::mutex pointLock;

    // Handling cursor drawing operations:
    // Holds cursor image data and draws it to the frame buffer.
    FBPainter::ImagePainter imagePainter;
    // Provides access to the frame buffer.
    FBPainter::FrameBuffer frameBuffer;

};
