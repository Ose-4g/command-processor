#include "keyboardinput.h"

namespace ose4g
{
    void KeyboardInput::enableKeyboard()
    {
        if (!enabled)
        {
            tcgetattr(STDIN_FILENO, &original);
            original.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
            tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
        }
        enabled = true;
    }

    void KeyboardInput::disableKeyboard()
    {
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
        enabled = false;
    }

    KeyboardInput &KeyboardInput::getInstance()
    {
        static KeyboardInput instance; // Thread-safe in C++11+
        return instance;
    }

    KeyboardInput::Input KeyboardInput::getInput()
    {
        while (true)
        {
            char c;
            if (read(STDIN_FILENO, &c, 1) == 1)
            {
                if (c == '\033')
                { // ESC or start of escape sequence
                    char seq[2];
                    if (read(STDIN_FILENO, &seq[0], 1) == 1 &&
                        read(STDIN_FILENO, &seq[1], 1) == 1)
                    {
                        if (seq[0] == '[')
                        {
                            switch (seq[1])
                            {
                            case 'A':
                                return {InputType::ARROW_UP, ' '};
                            case 'B':
                                return {InputType::ARROW_DOWN, ' '};
                            case 'C':
                                return {InputType::ARROW_RIGHT, ' '};
                            case 'D':
                                return {InputType::ARROW_LEFT, ' '};
                            }
                        }
                    }
                }
                else if (c == '\t')
                {
                    return {InputType::TAB, ' '};
                }
                else if (c == '\n')
                {
                    return {InputType::ENTER, ' '};
                }
                else if (c == 127 || c == '\b')
                {
                    return {InputType::BACKSPACE, ' '};
                }
                else
                {
                    return {InputType::ASCII, c};
                }
            }
        }
        return {InputType::INVALID_INPUT, ' '};
    }
}