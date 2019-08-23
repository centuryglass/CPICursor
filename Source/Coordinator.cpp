#include "Coordinator.h"
#include "Debug.h"
#include <chrono>
#include <ctime>


// Initializes the Coordinator, saving references to the objects the
// coordinator coordinates.
Coordinator::Coordinator(CursorPainter& painter, CursorTracker& tracker) :
        painter(painter), tracker(tracker), loopShouldContinue(true) { }


// Ensures the update thread has stopped before the Coordinator is destroyed.
Coordinator::~Coordinator()
{
    if (updateThread.joinable())
    {
        stopUpdateLoop();
        updateThread.join();
    }
}

// If its not already running, starts the cursor update loop in a new thread.
void Coordinator::startUpdateLoop(const int updatesPerSecond)
{
    if (! updateThread.joinable())
    {
        loopShouldContinue.store(true);
        updateThread = std::thread(cursorUpdateLoop, updatesPerSecond, this);
    }
}

// Signals to the Coordinator that the cursor update loop should stop.
void Coordinator::stopUpdateLoop()
{
    loopShouldContinue.store(false);
}


// Continually updates the cursor position at a specific frequency, running
// within another thread.
void Coordinator::cursorUpdateLoop
(const int updatesPerSecond, Coordinator* coordinator)
{
    using namespace std::chrono;
    using HighResClock = high_resolution_clock;
    using TimePoint = time_point<HighResClock, nanoseconds>;
    const nanoseconds loopDuration(1000000000 / updatesPerSecond);
    TimePoint lastUpdateTime(HighResClock::now());
    while(coordinator->loopShouldContinue.load())
    {
        const TimePoint loopStart = HighResClock::now();
        const nanoseconds timePassed = loopStart - lastUpdateTime;
        if (timePassed < loopDuration)
        {
            const nanoseconds sleepTime = loopDuration - timePassed;
            struct timespec sleepTimer;
            sleepTimer.tv_sec = 0;
            sleepTimer.tv_nsec = sleepTime.count();
            nanosleep(&sleepTimer, nullptr);
        }
        CursorTracker::Point cursorPos = coordinator->tracker.getCursorPos();
        coordinator->painter.drawCursor(cursorPos.x, cursorPos.y);
        lastUpdateTime = HighResClock::now();
    }
}


// Receives keyboard input events, passing them on to the cursor tracker.
void Coordinator::handleKeyEvent
(const KeyListener::Key key, const KeyDaemon::EventType actionType)
{
    CursorTracker::DirectionKey directionKey;
    switch (key)
    {
        case KeyListener::Key::up:
            directionKey = CursorTracker::DirectionKey::up;
            break;
        case KeyListener::Key::down:
            directionKey = CursorTracker::DirectionKey::down;
            break;
        case KeyListener::Key::left:
            directionKey = CursorTracker::DirectionKey::left;
            break;
        case KeyListener::Key::right:
            directionKey = CursorTracker::DirectionKey::right;
            break;
        default:
            return; // TODO: handle keys other than navigation keys!
    }
    const bool keyIsDown = (actionType == KeyDaemon::EventType::pressed)
            || (actionType == KeyDaemon::EventType::held);
    tracker.updateKeyState(directionKey, keyIsDown);
}
