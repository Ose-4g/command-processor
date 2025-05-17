#ifndef KEYBOARDINPUT_H
#define KEYBOARDINPUT_H

#include <termios.h>
#include <unistd.h>
#include <iostream>


namespace ose4g{

    class KeyboardInput{
        private:
            termios original;
            bool enabled;
            KeyboardInput(){}
        public:
            enum class InputType {
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
            void enableKeyboard();
            void disableKeyboard();
            Input getInput();
            static KeyboardInput& getInstance();

            KeyboardInput(const KeyboardInput&) = delete;
            KeyboardInput& operator=(const KeyboardInput&) = delete;
            KeyboardInput(KeyboardInput&&) = delete;
            KeyboardInput& operator=(KeyboardInput&&) = delete;
    };
}

#endif