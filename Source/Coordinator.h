/**
 * @file  Coordinator.h
 *
 * @brief  Passes information between CPICursor, the cursor painter daemon, and
 *         the key listener daemon.
 */

#pragma once
#include "KeyListener.h"
#include "CursorPainter.h"
#include "CursorTracker.h"
#include <atomic>
#include <thread>

class Coordinator : public KeyListener::InputHandler
{
public:
    /**
     * @brief  Initializes the Coordinator, saving references to the objects
     *         the coordinator coordinates.
     *
     * @param painter  The object responsible for sending signals to the
     *                 painter daemon.
     *
     * @param tracker  The object responsible for keeping track of where the
     *                 cursor should be.
     */
    Coordinator(CursorPainter& painter, CursorTracker& tracker);

    /**
     * @brief  Ensures the update thread has stopped before the Coordinator
     *         is destroyed.
     */
    virtual ~Coordinator();

    /**
     * @brief  If its not already running, starts the cursor update loop in a
     *         new thread.
     *
     * @param updatesPerSecond  Number of times per second that the Coordinator
     *                          should send cursor updates.
     */
    void startUpdateLoop(const int updatesPerSecond);

    /**
     * @brief  Signals to the Coordinator that the cursor update loop should
     *         stop.
     */
    void stopUpdateLoop();

private:
    /**
     * @brief  Continually updates the cursor position at a specific frequency,
     *         running within another thread.
     *
     * @param updatesPerSecond  Number of times per second that the Coordinator
     *                          should send cursor updates.
     *
     * @param coordinator       The coordinator used to start the loop.
     */
    static void cursorUpdateLoop
    (const int updatesPerSecond, Coordinator* coordinator);

    /**
     * @brief  Receives keyboard input events, passing them on to the cursor
     *         tracker.
     *
     * @param key         The type of key associated with the event.
     *
     * @param actionType  Whether the key was pressed, released, or held.
     */
    virtual void handleKeyEvent(const KeyListener::Key key,
            const KeyDaemon::EventType actionType) override;

    // Requests cursor drawing actions:
    CursorPainter& painter;
    // Tracks the position of the cursor:
    CursorTracker& tracker;
    // Runs the update loop:
    std::thread updateThread;
    // Whether the update loop should continue:
    std::atomic<bool> loopShouldContinue;
};


