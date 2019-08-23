/**
 * @file  Main.cpp
 *
 * @brief  The main build file for the CPICursor application.
 */

#include "CursorPainter.h"
#include "KeyListener.h"
#include "Coordinator.h"
#include "Debug.h"
#include <iostream>
#include <string>
#include <unistd.h>
#include <linux/input-event-codes.h>

// Temporary, only use for testing KeyDaemon:
class InputTester : public KeyListener::InputHandler
{
public:
    InputTester() { }

    virtual ~InputTester() { }

private:
    virtual void handleKeyEvent
    (const KeyListener::Key key, const KeyDaemon::EventType actionType) override
    {
        std::string keyName;
        switch (key)
        {
        case KeyListener::Key::up:
            keyName = "up";
            break;
        case KeyListener::Key::down:
            keyName = "down";
            break;
        case KeyListener::Key::left:
            keyName = "left";
            break;
        case KeyListener::Key::right:
            keyName = "right";
            break;
        case KeyListener::Key::leftClick:
            keyName = "leftClick";
            break;
        case KeyListener::Key::rightClick:
            keyName = "rightClick";
            break;
        case KeyListener::Key::exit:
            keyName = "exit";
            break;
        default:
            keyName = "invalid(";
            keyName += std::to_string(static_cast<int>(key)) + ")";
        }
        std::string eventName = KeyDaemon::getEventString(actionType);
        DBG(keyName << ": " << eventName);
    }
};

int main(int argc, char** argv)
{
    std::cout << "Starting cursor painter:\n";
    CursorPainter painter;
    while(painter.getDisplayWidth() == 0)
    {
        sleep(1);
    }
    CursorTracker tracker(0, 0, painter.getDisplayWidth(),
            painter.getDisplayHeight());
    Coordinator coordinator(painter, tracker);
    KeyListener listener(coordinator);
    listener.setKeyCode(KEY_UP, KeyListener::Key::up);
    listener.setKeyCode(KEY_DOWN, KeyListener::Key::down);
    listener.setKeyCode(KEY_LEFT, KeyListener::Key::left);
    listener.setKeyCode(KEY_RIGHT, KeyListener::Key::right);
    listener.setKeyCode(KEY_SPACE, KeyListener::Key::leftClick);
    listener.setKeyCode(KEY_RIGHTALT, KeyListener::Key::rightClick);
    listener.setKeyCode(KEY_ESC, KeyListener::Key::exit);
    listener.startKeyDaemon();
    coordinator.startUpdateLoop(60);
    sleep(30000);
    return 0;
}
