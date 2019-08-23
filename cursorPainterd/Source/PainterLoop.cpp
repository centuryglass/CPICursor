#include "PainterLoop.h"
#include "Cursor.h"
#include "CodeImage.h"
#include "Debug.h"
#include <ctime>

#ifdef DF_DEBUG
static const constexpr char* messagePrefix = "PainterLoop::";
#endif

static const constexpr int maxFPS = 60;
static const std::chrono::nanoseconds loopDuration(1000000000 / maxFPS);

// Initializes cursor image data on construction, and sends the display
// resolution back to CPICursor.
PainterLoop::PainterLoop() : DaemonFramework::DaemonLoop(sizeof(size_t) * 2),
    imagePainter(new FBPainter::CodeImage<FBPainter::Cursor>),
    frameBuffer(FB_PATH),
    lastDrawTime(std::chrono::high_resolution_clock::now()) 
{
    static size_t resolution [2];
    resolution[0] = frameBuffer.getWidth();
    resolution[1] = frameBuffer.getHeight();
    DF_DBG(messagePrefix << __func__ << ": sending display resolution "
            << resolution[0] << " x " << resolution[1] << " (size "
            << sizeof(resolution) << ") to CPICursor.");
    messageParent(reinterpret_cast<const unsigned char*>(&resolution),
            sizeof(resolution));
}


// Periodically checks for pending cursor drawing commands, redrawing the
// cursor no more than once per loop.
int PainterLoop::loopAction()
{
    using namespace std::chrono;
    const time_point<high_resolution_clock, nanoseconds> loopStart 
            = high_resolution_clock::now();
    const nanoseconds timePassed = loopStart - lastDrawTime;
    if (timePassed < loopDuration)
    {
        const nanoseconds sleepTime = loopDuration - timePassed;
        struct timespec sleepTimer;
        sleepTimer.tv_sec = 0;
        sleepTimer.tv_nsec = sleepTime.count();
        nanosleep(&sleepTimer, nullptr);
    }
    const std::lock_guard<std::mutex> lock(pointLock);
    if (gotFirstMessage)
    {
        DrawPoint& nextPoint = (bufferedPointCount == 0) ? lastDrawn 
                : pointBuffer[startIndex];
        if (lastDrawn.x != nextPoint.x || lastDrawn.y != nextPoint.y)
        {
            imagePainter.clearImage(&frameBuffer);
        }
        imagePainter.setImageOrigin(nextPoint.x, nextPoint.y, &frameBuffer);
        lastDrawn = nextPoint;
        if (bufferedPointCount > 0)
        {
            bufferedPointCount--;
            startIndex++;
            if (startIndex >= pointBufSize)
            {
                startIndex = 0;
            }
        }
    }
    lastDrawTime = high_resolution_clock::now();
    return 0;
}


// Reads cursor drawing coordinates sent from CPICursor, and buffers them until
// loopAction can handle them.
void PainterLoop::handleParentMessage
(const unsigned char* messageData, const size_t messageSize)
{
    if (messageSize != (sizeof(size_t) * 2))
    {
        DF_DBG(messagePrefix << __func__ << ": Invalid message size "
                << messageSize);
        return;
    }
    const size_t* pointMessage = reinterpret_cast<const size_t*>(messageData);
    DrawPoint point = { pointMessage[0], pointMessage[1] };
    DF_DBG_V(messagePrefix << __func__ << ": Requesting cursor draw at ("
            << point.x << ", " << point.y << ")");
    const std::lock_guard<std::mutex> lock(pointLock);
    gotFirstMessage = true;
    if (bufferedPointCount == pointBufSize)
    {
        // Buffer is full, cut the oldest point and push the index forward.
        pointBuffer[startIndex] = point;
        DF_DBG_V("Request buffered at " << startIndex);
        startIndex++;
        if (startIndex >= pointBufSize)
        {
            startIndex = 0;
        }
    }
    else
    {
        size_t bufferEndIdx = startIndex + bufferedPointCount;
        if (bufferEndIdx >= pointBufSize)
        {
            bufferEndIdx %= pointBufSize;
        }
        pointBuffer[bufferEndIdx] = point;
        DF_DBG_V("Request buffered at " << bufferEndIdx);
        bufferedPointCount++;
    }
}
