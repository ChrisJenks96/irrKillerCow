#pragma once

#include <engine.h>

using namespace engine;
using namespace core;
using namespace scene;
using namespace video;
using namespace io;
using namespace gui;

IGUIButton* shieldBtnToggle;
IGUIButton* nukeBtnToggle;

bool GUINukeToggle = false;
bool GUIShieldToggle = false;

/*class MyEventReceiver : public IEventReceiver
{
public:
    bool Keys[256];
    // We'll create a struct to record info on the mouse state
    struct SMouseState
    {
        core::position2di Position;
        bool LeftButtonDown;
        SMouseState() : LeftButtonDown(false) { }
    } MouseState;

    // This is the one method that we have to implement
    virtual bool OnEvent(const SEvent& event)
    {
        if (event.EventType == EET_GUI_EVENT)
        {
            int id = event.GUIEvent.Caller->getID();

            switch (event.GUIEvent.EventType)
            {
                case EGET_BUTTON_CLICKED:
                    switch (id)
                    {
                        case 234:
                            shieldBtnToggle->setVisible(false);
                            GUIShieldToggle = true;
                            break;
                        case 235:
                            nukeBtnToggle->setVisible(false);
                            GUINukeToggle = true;
                            break;
                    }
            }
        }
        if (event.EventType == irr::EET_KEY_INPUT_EVENT)
        {
            Keys[event.KeyInput.Key] = event.KeyInput.PressedDown;
        }
        // Remember the mouse state
        if (event.EventType == irr::EET_MOUSE_INPUT_EVENT)
        {
            switch (event.MouseInput.Event)
            {
            case EMIE_LMOUSE_PRESSED_DOWN:
                MouseState.LeftButtonDown = true;
                break;

            case EMIE_LMOUSE_LEFT_UP:
                MouseState.LeftButtonDown = false;
                break;

            case EMIE_MOUSE_MOVED:
                MouseState.Position.X = event.MouseInput.X;
                MouseState.Position.Y = event.MouseInput.Y;
                break;

            default:
                // We won't use the wheel
                break;
            }
        }

        // The state of each connected joystick is sent to us
        // once every run() of the Irrlicht device.  Store the
        // state of the first joystick, ignoring other joysticks.
        // This is currently only supported on Windows and Linux.
        if (event.EventType == irr::EET_JOYSTICK_INPUT_EVENT
            && event.JoystickEvent.Joystick == 0)
        {
            JoystickState = event.JoystickEvent;
        }

        return false;
    }

    const SEvent::SJoystickEvent& GetJoystickState(void) const
    {
        return JoystickState;
    }

    const SMouseState& GetMouseState(void) const
    {
        return MouseState;
    }


    MyEventReceiver()
    {
    }

private:
    SEvent::SJoystickEvent JoystickState;
};*/