/**
 * @file  CursorTracker.h
 *
 * @brief  Tracks cursor position, speed, and acceleration based on provided
 *         key input times.
 */

#pragma once
#include <chrono>
#include <mutex>

class CursorTracker
{
public:
    /**
     * @brief  Saves the display size and initial cursor position on
     *         construction.
     *
     * @param startX         The cursor's initial x-coordinate, measured in
     *                       pixels from the left side of the display.
     *
     * @param startY         The cursor's initial y-coordinate, measured in
     *                       pixels from the top of the display.
     *
     * @param displayWidth   The main display's width in pixels.
     *
     * @param displayHeight  The main display's height in pixels.
     */
    CursorTracker(const size_t startX, const size_t startY,
            const size_t displayWidth, const size_t displayHeight);

    virtual ~CursorTracker() { }

    /**
     * @brief  Represents the four directional input keys used to control the
     *         cursor.
     */
    enum class DirectionKey
    {
        up,
        down,
        left,
        right
    };

    /**
     * @brief  Updates the cursor tracker on the current state of a directional
     *         input key.
     *
     * @param key      The key type being updated.
     *
     * @param keyHeld  True if the key is now held down, false if it has been
     *                 released.
     */
    void updateKeyState(const DirectionKey key, const bool keyHeld);

    /**
     * @brief  A basic 2D point structure used to return a pixel coordinate.
     */
    struct Point
    {
        size_t x;
        size_t y;
    };
    
    /**
     * @brief  Gets the current position of the cursor on the display.
     *
     * @return  The cursor position, updated appropriately based on which 
     *          directional keys have been held down.
     */
    Point getCursorPos();

private:
    // Time measurement types used by CursorTracker:
    typedef std::chrono::high_resolution_clock UpdateClock;
    typedef std::chrono::milliseconds Duration;
    typedef std::chrono::time_point<UpdateClock, Duration> TimePoint;

    /**
     * @brief  Gets the current time using CursorTracker's time measurement
     *         types.
     *
     * @return  An object representing the current clock time.
     */
    static inline TimePoint getCurrentTime()
    {
        return std::chrono::time_point_cast<Duration>(UpdateClock::now());
    }

    // Ensures cursor access is threadsafe:
    std::mutex cursorLock;
    // Whether each direction key is currently held:
    bool heldKeys [4];
    // The last time each direction key was pressed or released:
    TimePoint lastUpdateTimes [4];
    // The last recorded position of the cursor, and the last time that
    // position was updated:
    Point cursorPt;
    TimePoint lastCursorUpdate;
    // The display dimensions:
    const size_t displayWidth;
    const size_t displayHeight;
};
