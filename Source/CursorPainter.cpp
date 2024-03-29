#include "CursorPainter.h"
#include "Debug.h"

#ifdef DEBUG
static const constexpr char* messagePrefix = "CursorPainter::";
#endif


// Launches the cursor painter daemon and prepares to send it commands.
CursorPainter::CursorPainter() :
DaemonFramework::DaemonControl(PAINTERD_PATH, PAINTERD_INPUT_PIPE_PATH,
        PAINTERD_OUTPUT_PIPE_PATH, sizeof(size_t) * 2)
{
    DBG_V(messagePrefix << __func__ << ": Starting cursorPainterd:");
    startDaemon({}, &listener);
    DBG_V(messagePrefix << __func__ << ": cursorPainterd started.");
}


// Ensures the cursor painter daemon is stopped on destruction.
CursorPainter::~CursorPainter()
{
    DBG_V(messagePrefix << __func__ << ": Stopping cursorPainterd:");
    stopDaemon();
    DBG_V(messagePrefix << __func__ << ": cursorPainterd stopped.");
}


// Commands the cursor painter daemon to draw the cursor at a specific
// coordinate.
bool CursorPainter::drawCursor(const size_t x, const size_t y)
{
    if (! isDaemonRunning())
    {
        DBG(messagePrefix << __func__
                << ": Daemon not running, trying to restart:");
        startDaemon({});
        if (! isDaemonRunning())
        {
            DBG(messagePrefix << __func__ << ": Starting daemon failed!");
            return false;
        }
    }
    const size_t coordinates [2] = {x, y};
    messageParent(reinterpret_cast<const unsigned char*>(coordinates),
            sizeof(size_t) * 2);
    return true;
}


// Gets the main display's width in pixels.
size_t CursorPainter::getDisplayWidth() const
{
    return listener.getDisplayWidth();
}


// Gets the main display's height in pixels.
size_t CursorPainter::getDisplayHeight() const
{
    return listener.getDisplayHeight();
}
