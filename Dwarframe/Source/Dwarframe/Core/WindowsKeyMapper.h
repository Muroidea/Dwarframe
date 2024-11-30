#ifndef WINDOWS_KEY_MAPPER_H
#define WINDOWS_KEY_MAPPER_H

#include "Dwarframe/Core/KeyCodes.h"

#include <map>

namespace Dwarframe {

	class WindowsKeyMapper
    {
    public:
        WindowsKeyMapper()
        {
            // Keyboard
            m_WindowsToEngineKeyboard[VK_BACK] = EDwarframeKeyCodes::DF_KEY_BACKSPACE;
            m_WindowsToEngineKeyboard[VK_TAB] = EDwarframeKeyCodes::DF_KEY_TAB;
            m_WindowsToEngineKeyboard[VK_RETURN] = EDwarframeKeyCodes::DF_KEY_ENTER;
            m_WindowsToEngineKeyboard[VK_SHIFT] = EDwarframeKeyCodes::DF_KEY_SHIFT;
            m_WindowsToEngineKeyboard[VK_CONTROL] = EDwarframeKeyCodes::DF_KEY_CTRL;
            m_WindowsToEngineKeyboard[VK_MENU] = EDwarframeKeyCodes::DF_KEY_ALT;
            m_WindowsToEngineKeyboard[VK_PAUSE] = EDwarframeKeyCodes::DF_KEY_PAUSE;
            m_WindowsToEngineKeyboard[VK_CAPITAL] = EDwarframeKeyCodes::DF_KEY_CAPSLOCK;
            m_WindowsToEngineKeyboard[VK_ESCAPE] = EDwarframeKeyCodes::DF_KEY_ESCAPE;
            m_WindowsToEngineKeyboard[VK_SPACE] = EDwarframeKeyCodes::DF_KEY_SPACEBAR;
            m_WindowsToEngineKeyboard[VK_PRIOR] = EDwarframeKeyCodes::DF_KEY_PAGEUP;
            m_WindowsToEngineKeyboard[VK_NEXT] = EDwarframeKeyCodes::DF_KEY_PAGEDOWN;
            m_WindowsToEngineKeyboard[VK_END] = EDwarframeKeyCodes::DF_KEY_END;
            m_WindowsToEngineKeyboard[VK_HOME] = EDwarframeKeyCodes::DF_KEY_HOME;
            m_WindowsToEngineKeyboard[VK_LEFT] = EDwarframeKeyCodes::DF_KEY_LEFT_ARROW;
            m_WindowsToEngineKeyboard[VK_UP] = EDwarframeKeyCodes::DF_KEY_UP_ARROW;
            m_WindowsToEngineKeyboard[VK_RIGHT] = EDwarframeKeyCodes::DF_KEY_RIGHT_ARROW;
            m_WindowsToEngineKeyboard[VK_DOWN] = EDwarframeKeyCodes::DF_KEY_DOWN_ARROW;
            m_WindowsToEngineKeyboard[VK_SNAPSHOT] = EDwarframeKeyCodes::DF_KEY_PRINTSCREEN;
            m_WindowsToEngineKeyboard[VK_INSERT] = EDwarframeKeyCodes::DF_KEY_INSERT;
            m_WindowsToEngineKeyboard[VK_DELETE] = EDwarframeKeyCodes::DF_KEY_DELETE;

            m_WindowsToEngineKeyboard[0x30] = EDwarframeKeyCodes::DF_KEY_NUM_0;
            m_WindowsToEngineKeyboard[0x31] = EDwarframeKeyCodes::DF_KEY_NUM_1;
            m_WindowsToEngineKeyboard[0x32] = EDwarframeKeyCodes::DF_KEY_NUM_2;
            m_WindowsToEngineKeyboard[0x33] = EDwarframeKeyCodes::DF_KEY_NUM_3;
            m_WindowsToEngineKeyboard[0x34] = EDwarframeKeyCodes::DF_KEY_NUM_4;
            m_WindowsToEngineKeyboard[0x35] = EDwarframeKeyCodes::DF_KEY_NUM_5;
            m_WindowsToEngineKeyboard[0x36] = EDwarframeKeyCodes::DF_KEY_NUM_6;
            m_WindowsToEngineKeyboard[0x37] = EDwarframeKeyCodes::DF_KEY_NUM_7;
            m_WindowsToEngineKeyboard[0x38] = EDwarframeKeyCodes::DF_KEY_NUM_8;
            m_WindowsToEngineKeyboard[0x39] = EDwarframeKeyCodes::DF_KEY_NUM_9;

            m_WindowsToEngineKeyboard[0x41] = EDwarframeKeyCodes::DF_KEY_A;
            m_WindowsToEngineKeyboard[0x42] = EDwarframeKeyCodes::DF_KEY_B;
            m_WindowsToEngineKeyboard[0x43] = EDwarframeKeyCodes::DF_KEY_C;
            m_WindowsToEngineKeyboard[0x44] = EDwarframeKeyCodes::DF_KEY_D;
            m_WindowsToEngineKeyboard[0x45] = EDwarframeKeyCodes::DF_KEY_E;
            m_WindowsToEngineKeyboard[0x46] = EDwarframeKeyCodes::DF_KEY_F;
            m_WindowsToEngineKeyboard[0x47] = EDwarframeKeyCodes::DF_KEY_G;
            m_WindowsToEngineKeyboard[0x48] = EDwarframeKeyCodes::DF_KEY_H;
            m_WindowsToEngineKeyboard[0x49] = EDwarframeKeyCodes::DF_KEY_I;
            m_WindowsToEngineKeyboard[0x4A] = EDwarframeKeyCodes::DF_KEY_J;
            m_WindowsToEngineKeyboard[0x4B] = EDwarframeKeyCodes::DF_KEY_K;
            m_WindowsToEngineKeyboard[0x4C] = EDwarframeKeyCodes::DF_KEY_L;
            m_WindowsToEngineKeyboard[0x4D] = EDwarframeKeyCodes::DF_KEY_M;
            m_WindowsToEngineKeyboard[0x4E] = EDwarframeKeyCodes::DF_KEY_N;
            m_WindowsToEngineKeyboard[0x4F] = EDwarframeKeyCodes::DF_KEY_O;
            m_WindowsToEngineKeyboard[0x50] = EDwarframeKeyCodes::DF_KEY_P;
            m_WindowsToEngineKeyboard[0x51] = EDwarframeKeyCodes::DF_KEY_Q;
            m_WindowsToEngineKeyboard[0x52] = EDwarframeKeyCodes::DF_KEY_R;
            m_WindowsToEngineKeyboard[0x53] = EDwarframeKeyCodes::DF_KEY_S;
            m_WindowsToEngineKeyboard[0x54] = EDwarframeKeyCodes::DF_KEY_T;
            m_WindowsToEngineKeyboard[0x55] = EDwarframeKeyCodes::DF_KEY_U;
            m_WindowsToEngineKeyboard[0x56] = EDwarframeKeyCodes::DF_KEY_V;
            m_WindowsToEngineKeyboard[0x57] = EDwarframeKeyCodes::DF_KEY_W;
            m_WindowsToEngineKeyboard[0x58] = EDwarframeKeyCodes::DF_KEY_X;
            m_WindowsToEngineKeyboard[0x59] = EDwarframeKeyCodes::DF_KEY_Y;
            m_WindowsToEngineKeyboard[0x5A] = EDwarframeKeyCodes::DF_KEY_Z;

            m_WindowsToEngineKeyboard[VK_NUMPAD0] = EDwarframeKeyCodes::DF_KEY_NUM_0;
            m_WindowsToEngineKeyboard[VK_NUMPAD1] = EDwarframeKeyCodes::DF_KEY_NUM_1;
            m_WindowsToEngineKeyboard[VK_NUMPAD2] = EDwarframeKeyCodes::DF_KEY_NUM_2;
            m_WindowsToEngineKeyboard[VK_NUMPAD3] = EDwarframeKeyCodes::DF_KEY_NUM_3;
            m_WindowsToEngineKeyboard[VK_NUMPAD4] = EDwarframeKeyCodes::DF_KEY_NUM_4;
            m_WindowsToEngineKeyboard[VK_NUMPAD5] = EDwarframeKeyCodes::DF_KEY_NUM_5;
            m_WindowsToEngineKeyboard[VK_NUMPAD6] = EDwarframeKeyCodes::DF_KEY_NUM_6;
            m_WindowsToEngineKeyboard[VK_NUMPAD7] = EDwarframeKeyCodes::DF_KEY_NUM_7;
            m_WindowsToEngineKeyboard[VK_NUMPAD8] = EDwarframeKeyCodes::DF_KEY_NUM_8;
            m_WindowsToEngineKeyboard[VK_NUMPAD9] = EDwarframeKeyCodes::DF_KEY_NUM_9;

            m_WindowsToEngineKeyboard[VK_MULTIPLY] = EDwarframeKeyCodes::DF_KEY_MULTIPLY;
            m_WindowsToEngineKeyboard[VK_ADD] = EDwarframeKeyCodes::DF_KEY_ADD;
            m_WindowsToEngineKeyboard[VK_SUBTRACT] = EDwarframeKeyCodes::DF_KEY_SUBTRACT;
            m_WindowsToEngineKeyboard[VK_DECIMAL] = EDwarframeKeyCodes::DF_KEY_DECIMAL;
            m_WindowsToEngineKeyboard[VK_DIVIDE] = EDwarframeKeyCodes::DF_KEY_DIVIDE;

            m_WindowsToEngineKeyboard[VK_F1] = EDwarframeKeyCodes::DF_KEY_F1;
            m_WindowsToEngineKeyboard[VK_F2] = EDwarframeKeyCodes::DF_KEY_F2;
            m_WindowsToEngineKeyboard[VK_F3] = EDwarframeKeyCodes::DF_KEY_F3;
            m_WindowsToEngineKeyboard[VK_F4] = EDwarframeKeyCodes::DF_KEY_F4;
            m_WindowsToEngineKeyboard[VK_F5] = EDwarframeKeyCodes::DF_KEY_F5;
            m_WindowsToEngineKeyboard[VK_F6] = EDwarframeKeyCodes::DF_KEY_F6;
            m_WindowsToEngineKeyboard[VK_F7] = EDwarframeKeyCodes::DF_KEY_F7;
            m_WindowsToEngineKeyboard[VK_F8] = EDwarframeKeyCodes::DF_KEY_F8;
            m_WindowsToEngineKeyboard[VK_F9] = EDwarframeKeyCodes::DF_KEY_F9;
            m_WindowsToEngineKeyboard[VK_F10] = EDwarframeKeyCodes::DF_KEY_F10;
            m_WindowsToEngineKeyboard[VK_F11] = EDwarframeKeyCodes::DF_KEY_F11;
            m_WindowsToEngineKeyboard[VK_F12] = EDwarframeKeyCodes::DF_KEY_F12;
            m_WindowsToEngineKeyboard[VK_F13] = EDwarframeKeyCodes::DF_KEY_F13;
            m_WindowsToEngineKeyboard[VK_F14] = EDwarframeKeyCodes::DF_KEY_F14;
            m_WindowsToEngineKeyboard[VK_F15] = EDwarframeKeyCodes::DF_KEY_F15;
            m_WindowsToEngineKeyboard[VK_F16] = EDwarframeKeyCodes::DF_KEY_F16;
            m_WindowsToEngineKeyboard[VK_F17] = EDwarframeKeyCodes::DF_KEY_F17;
            m_WindowsToEngineKeyboard[VK_F18] = EDwarframeKeyCodes::DF_KEY_F18;
            m_WindowsToEngineKeyboard[VK_F19] = EDwarframeKeyCodes::DF_KEY_F19;
            m_WindowsToEngineKeyboard[VK_F20] = EDwarframeKeyCodes::DF_KEY_F20;
            m_WindowsToEngineKeyboard[VK_F21] = EDwarframeKeyCodes::DF_KEY_F21;
            m_WindowsToEngineKeyboard[VK_F22] = EDwarframeKeyCodes::DF_KEY_F22;
            m_WindowsToEngineKeyboard[VK_F23] = EDwarframeKeyCodes::DF_KEY_F23;
            m_WindowsToEngineKeyboard[VK_F24] = EDwarframeKeyCodes::DF_KEY_F24;

            m_WindowsToEngineKeyboard[VK_NUMLOCK] = EDwarframeKeyCodes::DF_KEY_NUMLOCK;
            m_WindowsToEngineKeyboard[VK_SCROLL] = EDwarframeKeyCodes::DF_KEY_SCROLL_LOCK;

            m_WindowsToEngineKeyboard[VK_LSHIFT] = EDwarframeKeyCodes::DF_KEY_LEFT_SHIFT;
            m_WindowsToEngineKeyboard[VK_RSHIFT] = EDwarframeKeyCodes::DF_KEY_RIGHT_SHIFT;
            m_WindowsToEngineKeyboard[VK_LCONTROL] = EDwarframeKeyCodes::DF_KEY_LEFT_CTRL;
            m_WindowsToEngineKeyboard[VK_RCONTROL] = EDwarframeKeyCodes::DF_KEY_RIGHT_CTRL;
            m_WindowsToEngineKeyboard[VK_LMENU] = EDwarframeKeyCodes::DF_KEY_LEFT_ALT;
            m_WindowsToEngineKeyboard[VK_RMENU] = EDwarframeKeyCodes::DF_KEY_RIGHT_ALT;

            m_WindowsToEngineKeyboard[VK_OEM_1] = EDwarframeKeyCodes::DF_KEY_SEMICOLON;
            m_WindowsToEngineKeyboard[VK_OEM_PLUS] = EDwarframeKeyCodes::DF_KEY_PLUS;
            m_WindowsToEngineKeyboard[VK_OEM_COMMA] = EDwarframeKeyCodes::DF_KEY_COMMA;
            m_WindowsToEngineKeyboard[VK_OEM_MINUS] = EDwarframeKeyCodes::DF_KEY_MINUS;
            m_WindowsToEngineKeyboard[VK_OEM_PERIOD] = EDwarframeKeyCodes::DF_KEY_PERIOD;
            m_WindowsToEngineKeyboard[VK_OEM_2] = EDwarframeKeyCodes::DF_KEY_SLASH;
            m_WindowsToEngineKeyboard[VK_OEM_3] = EDwarframeKeyCodes::DF_KEY_TILDE;
            m_WindowsToEngineKeyboard[VK_OEM_4] = EDwarframeKeyCodes::DF_KEY_LEFT_BRACKET;
            m_WindowsToEngineKeyboard[VK_OEM_5] = EDwarframeKeyCodes::DF_KEY_BACKSLASH;
            m_WindowsToEngineKeyboard[VK_OEM_6] = EDwarframeKeyCodes::DF_KEY_RIGHT_BRACKET;
            m_WindowsToEngineKeyboard[VK_OEM_7] = EDwarframeKeyCodes::DF_KEY_APOSTROPHE;

            // Mouse
            m_WindowsToEngineMouse[RI_MOUSE_LEFT_BUTTON_DOWN] = EDwarframeKeyCodes::DF_MOUSE_LEFT_BUTTON;
            m_WindowsToEngineMouse[RI_MOUSE_LEFT_BUTTON_UP] = EDwarframeKeyCodes::DF_MOUSE_LEFT_BUTTON;
            m_WindowsToEngineMouse[RI_MOUSE_RIGHT_BUTTON_DOWN] = EDwarframeKeyCodes::DF_MOUSE_RIGHT_BUTTON;
            m_WindowsToEngineMouse[RI_MOUSE_RIGHT_BUTTON_UP] = EDwarframeKeyCodes::DF_MOUSE_RIGHT_BUTTON;
            m_WindowsToEngineMouse[RI_MOUSE_MIDDLE_BUTTON_DOWN] = EDwarframeKeyCodes::DF_MOUSE_MIDDLE_BUTTON;
            m_WindowsToEngineMouse[RI_MOUSE_MIDDLE_BUTTON_UP] = EDwarframeKeyCodes::DF_MOUSE_MIDDLE_BUTTON;
            m_WindowsToEngineMouse[RI_MOUSE_BUTTON_4_DOWN] = EDwarframeKeyCodes::DF_MOUSE_X1_BUTTON;
            m_WindowsToEngineMouse[RI_MOUSE_BUTTON_4_UP] = EDwarframeKeyCodes::DF_MOUSE_X1_BUTTON;
            m_WindowsToEngineMouse[RI_MOUSE_BUTTON_5_DOWN] = EDwarframeKeyCodes::DF_MOUSE_X2_BUTTON;
            m_WindowsToEngineMouse[RI_MOUSE_BUTTON_5_UP] = EDwarframeKeyCodes::DF_MOUSE_X2_BUTTON;
        }

    public:
        [[nodiscard]] inline EDwarframeKeyCodes MapKeyboardCode(uint16 WindowsCode)
        {
            std::map<uint16, EDwarframeKeyCodes>::iterator Result = m_WindowsToEngineKeyboard.find(WindowsCode);
            if (Result != m_WindowsToEngineKeyboard.end())
            {
                return Result->second;
            }

            return EDwarframeKeyCodes::UNKNOWN;
        }

        [[nodiscard]] inline EDwarframeKeyCodes MapMouseCode(uint16 WindowsCode)
        {
            std::map<uint16, EDwarframeKeyCodes>::iterator Result = m_WindowsToEngineMouse.find(WindowsCode);
            if (Result != m_WindowsToEngineMouse.end())
            {
                return Result->second;
            }

            return EDwarframeKeyCodes::UNKNOWN;
        }

    private:
        std::map<uint16, EDwarframeKeyCodes> m_WindowsToEngineKeyboard;
        std::map<uint16, EDwarframeKeyCodes> m_WindowsToEngineMouse;
    };

}

#endif

