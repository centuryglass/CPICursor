#include "CursorTracker.h"
#include <algorithm>
#include <iostream>

// Cursor speed and acceleration, measured in pixels per millisecond and pixels
// per millisecond^2. TODO: load these from a config file.
static const constexpr double minSpeed = 0.05;
static const constexpr double maxSpeed = 1.0;
// Default acceleration: reach max speed in six seconds.
// s = .5at^2 -> a = 2s/t^2
static const constexpr size_t timeToMax = 6000;
static const constexpr double accel = (maxSpeed - minSpeed)
        / (double) timeToMax;



// Saves the display size and initial cursor position on construction.
CursorTracker::CursorTracker(
        const size_t startX,
        const size_t startY,
        const size_t displayWidth,
        const size_t displayHeight) :
    displayWidth(displayWidth),
    displayHeight(displayHeight)
{
    std::cout << "accel = " << accel << " p/ms^2\n";
    cursorPt = { startX, startY };
    TimePoint currentTime = getCurrentTime();
    lastCursorUpdate = currentTime;
    for (int i = 0; i < 4; i++)
    {
        heldKeys[i] = false;
        lastUpdateTimes[i] = currentTime;
    }
}


// Updates the cursor tracker on the current state of a directional input key.
void CursorTracker::updateKeyState(const DirectionKey key, const bool keyHeld)
{
    const int keyIdx = static_cast<int>(key);
    if (keyHeld == heldKeys[keyIdx])
    {
        return; // No action needed if nothing changed.
    }
    TimePoint currentTime = getCurrentTime();
    // Update saved positions when keys are released:
    Point newPt;
    if (! keyHeld)
    {
        newPt = getCursorPos();
    }
    std::lock_guard<std::mutex> lock(cursorLock);
    if (! keyHeld)
    {
        cursorPt = newPt;
        lastCursorUpdate = currentTime;
    }
    heldKeys[keyIdx] = keyHeld;
    lastUpdateTimes[keyIdx] = currentTime;
}


// Gets the current position of the cursor on the display.
CursorTracker::Point CursorTracker::getCursorPos()
{
    std::lock_guard<std::mutex> lock(cursorLock);
    using namespace std::chrono;
    TimePoint currentTime = getCurrentTime();

    // Find a distance moved based on default speed and acceleration values,
    // and from how long the acceleration key has been held.
    const auto getOffset = [&currentTime, this] (const TimePoint buttonPressed)
    {
        double offset = 0;
        // The position the offset will be applied to was saved at
        // lastCursorUpdate. If the button has been held since before that
        // point, find initial speed:
        double v0 = minSpeed;
        size_t untilVMax = timeToMax;
        size_t msSinceUpdate;
        if (buttonPressed < lastCursorUpdate)
        {
            msSinceUpdate = (currentTime - lastCursorUpdate).count();
            Duration accelTime = lastCursorUpdate - buttonPressed;
            v0 = std::min(maxSpeed, minSpeed + accel * accelTime.count());
            if (accelTime.count() > timeToMax)
            {
                untilVMax = 0;
            }
            else
            {
                untilVMax -= accelTime.count();
            }
        }
        else
        {
            msSinceUpdate = (currentTime - buttonPressed).count();
        }
        size_t accelTime = std::min(msSinceUpdate, untilVMax);
        if (accelTime > 0)
        {
            offset = (v0 * accelTime) + (0.5 * accel * accelTime * accelTime);
        }
        if (msSinceUpdate > accelTime)
        {
            offset += maxSpeed * (msSinceUpdate - accelTime);
        }
        return offset;
    };

    // Find a new cursor position along an axis, given the amount of time that
    // the positive and negative acceleration keys have been held.
    const auto getNewPos = [&getOffset, &currentTime] (const size_t initialPos,
            const size_t maxPos,
            const Duration negKeyHeld,
            const Duration posKeyHeld)
    {
        double totalOffset = 0;
        Duration durations [2] = { negKeyHeld, posKeyHeld };
        int multipliers [2] = { -1, 1 };
        for (int i = 0; i < 2; i++)
        {
            if (durations[i].count() == 0)
            {
                continue;
            }
            TimePoint buttonPressed = currentTime - durations[i];
            totalOffset += (getOffset(buttonPressed) * multipliers[i]);
        }
        double result = totalOffset + initialPos;
        if (result < 0)
        {
            return static_cast<size_t>(0);
        }
        if (result >= maxPos)
        {
            return maxPos;
        }
        return static_cast<size_t>(result);
    };

    // Use key press times and whether keys are held to get the duration that
    // each key has been held:
    Duration durations [4];
    for (int i = 0; i < 4; i++)
    {
        if (! heldKeys[i])
        {
            durations[i] = Duration(0);
        }
        else
        {
            durations[i] = currentTime - lastUpdateTimes[i];
        }
    }
    // Apply durations and getNewPos to update x and y coordinates.
    Point pos = 
    {
        getNewPos(cursorPt.x, displayWidth,
                durations[static_cast<int>(DirectionKey::left)],
                durations[static_cast<int>(DirectionKey::right)]),

        getNewPos(cursorPt.y, displayHeight,
                durations[static_cast<int>(DirectionKey::up)],
                durations[static_cast<int>(DirectionKey::down)])
    };
    return pos;
}
