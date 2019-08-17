/**
 * @file  KeyListener.h
 *
 * @brief  Launches the daemon that monitors keyboard input, and processes
 *         data sent by the daemon.
 */

#pragma once
#include "Controller.h"
#include <map>

class KeyListener : protected KeyDaemon::Controller
{
public:
    /**
     * @brief  Lists all types of key input required by CPICursor.
     */
    enum class Key
    {
        up,
        down,
        left,
        right,
        leftClick,
        rightClick,
        exit
    };

    /**
     * @brief  An abstract interface for classes that handle CPICursor input
     *         events.
     */
    class InputHandler
    {
    public:
        friend KeyListener;

        InputHandler() { }

        virtual ~InputHandler() { }

    private: 
        /**
         * @brief  Takes whatever steps are necessary to properly handle a
         *         keyboard input event.
         *
         * @param key         The type of key associated with the event.
         *
         * @param actionType  Whether the key was pressed, released, or held.
         */
        virtual void handleKeyEvent
        (const Key key, const KeyDaemon::EventType actionType) = 0;
    };

    
    /**
     * @brief  Stores the InputHandler on construction.
     */
    KeyListener(InputHandler& inputHandler);

    virtual ~KeyListener() { }


    /**
     * @brief  Assigns an input Key type to a specific linux keyboard code.
     *         All required keys must be assigned before starting the daemon.
     *
     * @param inputCode  A Linux keyboard input code (as defined in
     *                   <linux/input-event-codes.h>) that the daemon will
     *                   track.
     *
     * @param keyType    The key input type that will be associated with that
     *                   key code.
     */
    void setKeyCode(const int inputCode, const Key keyType);

    /**
     * @brief  Starts the key daemon, having it track all input codes provided
     *         using setKeyCode.
     */
    void startKeyDaemon();

    // Grant limited access to DaemonControl public methods:
    using DaemonFramework::DaemonControl::stopDaemon;
    using DaemonFramework::DaemonControl::isDaemonRunning;
    using DaemonFramework::DaemonControl::getDaemonProcessID;
    using DaemonFramework::DaemonControl::getExitCode;

private:
    /**
     * @brief  Passes valid key event data on to the InputHandler.
     *
     * @param keyMessage  A key event message sent by the daemon.
     */
    virtual void handleKeyEvent(const KeyDaemon::KeyMessage& keyMessage)
        override;

    // Object responsible for deciding what to do with input events:
    InputHandler& inputHandler;
    // Maps key code numbers to the Key type they control:
    std::map<int, Key> keyCodes;
};
