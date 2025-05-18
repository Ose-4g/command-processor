#ifndef KEYBOARDINPUT_H
#define KEYBOARDINPUT_H

#include <termios.h>
#include <unistd.h>
#include <iostream>

namespace ose4g
{
    /// @brief Singleton class to get keyboard input.
    class KeyboardInput
    {
    private:
        termios original;
        bool enabled;

        /// @brief private constructor
        KeyboardInput() {}

    public:
        enum class InputType
        {
            ASCII,
            TAB,
            BACKSPACE,
            ARROW_LEFT,
            ARROW_RIGHT,
            ARROW_UP,
            ARROW_DOWN,
            ENTER,
            INVALID_INPUT
        };

        using Input = std::pair<InputType, char>;

        /// @brief enable raw terminal mode
        void enableKeyboard();

        /// @brief disable raw terminal mode
        void disableKeyboard();

        /// @brief get input pressed by user on keyboard
        /// @return the a pair of InputType and a char. If the input is ASCII, the character will be the input
        /// else the char will be empty
        Input getInput();

        /// @brief get singleton instance
        /// @return singleton instance
        static KeyboardInput &getInstance();

        // delete functions to keep this class a singleton
        KeyboardInput(const KeyboardInput &) = delete;
        KeyboardInput &operator=(const KeyboardInput &) = delete;
        KeyboardInput(KeyboardInput &&) = delete;
        KeyboardInput &operator=(KeyboardInput &&) = delete;
    };
}

#endif