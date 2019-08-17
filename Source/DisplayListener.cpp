#include "DisplayListener.h"
#include "Debug.h"

#ifdef DEBUG
// Print the full class name before all debug output:
static const constexpr char* messagePrefix = "DisplayListener::";
#endif

// Gets the main display's width in pixels.
size_t DisplayListener::getDisplayWidth() const
{
    return width;
}


// Gets the main display's height in pixels.
size_t DisplayListener::getDisplayHeight() const
{
    return height;
}


// Read the display resolution sent by the cursor painter daemon.
void DisplayListener::processData
(const unsigned char* data, const size_t size)
{
    if (size != (sizeof(size_t) * 2))
    {
        DBG(messagePrefix << __func__
                << ": Ignoring message with invalid size " << size);
        return;
    }
    const size_t* screenData = reinterpret_cast<const size_t*>(data);
    width = screenData[0];
    height = screenData[1];
    DBG(messagePrefix << __func__ << ": Received display resolution "
            << width << " x " << height << " from cursorPainterd.");
}
