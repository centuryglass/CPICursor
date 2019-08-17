#include "KeyListener.h"
#include "Debug.h"

#ifdef DEBUG
// Print the full class name before all debug output:
static const constexpr char* messagePrefix = "KeyListener::";
#endif


// Stores the InputHandler on construction.
KeyListener::KeyListener(InputHandler& inputHandler) :
    inputHandler(inputHandler) { }


// Assigns an input Key type to a specific linux keyboard code.
void KeyListener::setKeyCode(const int inputCode, const Key keyType)
{
    keyCodes[inputCode] = keyType;
}


// Starts the key daemon, having it track all input codes provided using
// setKeyCode.
void KeyListener::startKeyDaemon()
{
    if (isDaemonRunning())
    {
        DBG(messagePrefix << __func__ << ": KeyDaemon is already running!");
        return;
    }
    std::vector<int> trackedCodes;
    for (const auto& iter : keyCodes)
    {
        trackedCodes.push_back(iter.first);
    }
    DBG_ASSERT(trackedCodes.size() == KD_KEY_LIMIT);
    KeyDaemon::Controller::startKeyDaemon(trackedCodes);
}


// Passes valid key event data on to the InputHandler.
void KeyListener::handleKeyEvent(const KeyDaemon::KeyMessage& keyMessage)
{
    try
    {
        const Key keyType = keyCodes.at(keyMessage.keyCode);
        inputHandler.handleKeyEvent(keyType, keyMessage.event);
    }
    catch (const std::out_of_range& e)
    {
        DBG(messagePrefix << __func__ << ": Received invalid key code "
                << keyMessage.keyCode);
    }
}
