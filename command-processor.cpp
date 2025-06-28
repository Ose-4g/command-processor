#include "command-processor.h"
#include "util.h"
#include <iostream>
#include <format>
#include <regex>
#include <exception>
#include "keyboardinput.h"

namespace ose4g
{
    // why is it 'd_name' and not 'm_name'
    CommandProcessorImpl::CommandProcessorImpl(const std::string &name) : d_name(name) {}

    void CommandProcessorImpl::help()
    {
        std::string helpMessage = "\t" + addColor("help", Color::BLUE) + ": lists all commands and their description";
        //std::ostringstream oss;
        //oss << "\t" << addColor("help", Color::BLUE) << ": lists all commands and their description\n";
        helpMessage += "\n\t" + addColor("clear", Color::BLUE) + ": clear screen";
        helpMessage += "\n\t" + addColor("exit", Color::BLUE) + ": exit program";
        helpMessage += "\n\t" + addColor("history", Color::BLUE) + ": print history";
        for (auto &command : d_commandDescriptionMap)
        {
            helpMessage += ("\n\t" + addColor(command.first, Color::BLUE) + ": " + command.second);
        }
        std::cout << helpMessage << std::endl;
    }
    void CommandProcessorImpl::add(const Command &command, std::function<void(const Args &)> processor, const std::string &description)
    {
        // starts with alphabet.
        // has alphanumeric characters or -
        std::regex commandPattern("^[A-Za-z][A-Za-z0-9-]*$");
        if (!std::regex_match(command, commandPattern))
        {
            throw std::invalid_argument("invalid argument provided for command");
        }
        if (auto it = d_commandProcessorMap.find(command); it != d_commandProcessorMap.end())
        {
            throw std::invalid_argument("Command already exists");
        }
        d_commandProcessorMap[command] = processor;
        d_commandDescriptionMap[command] = description;
    }

    void CommandProcessorImpl::add(const Command &command, std::function<void(const Args &)> processor, const std::vector<Rule *> &validateRules, const std::string &description)
    {
        add(command, processor, description);
        d_commandRuleMap[command] = validateRules;
    }

    void CommandProcessorImpl::run()
    {
        
        clearScreen(true);
        std::string input;
        while (isRunning)
        {
            KeyboardInput::getInstance().enableKeyboard();
            input = getUserInput();
            KeyboardInput::getInstance().disableKeyboard();
            d_history.addBack(input);
            Command command;
            Args args;
            if (!parseStatement(input, command, args))
            {
                std::cout << addColor("Invalid input", Color::RED) << std::endl;
                continue;
            }
            try
            {
                process(command, args);
                std::cout << std::endl;
            }
            catch (const std::invalid_argument &exc)
            {
                std::cout << addColor(exc.what(), Color::RED) << std::endl;
            }
            catch (const std::exception &exc)
            {
                std::cout << addColor(exc.what(), Color::RED) << std::endl;
            }
            catch (...)
            {
                std::cout << addColor("An unknown error occured", Color::RED) << std::endl;
            }

        }
    }

    bool CommandProcessorImpl::parseStatement(const std::string &input, Command &command, Args &args)
    {
        if (input == "")
        {
            return true;
        }

        std::vector<std::string> seen;
        std::string temp = "";
        int i = 0;
        int n = input.size();
        while (i < n)
        {
            // if string is within quotes, find the end quote.
            if (input[i] == '"' || input[i] == '\'')
            {
                int start = i;
                char c = input[i];
                i++;
                while (i < n && input[i] != c)
                {
                    i++;
                }
                if (i == n)
                {
                    return false;
                }
                seen.push_back(input.substr(start + 1, i - start - 1));
            }

            else if (input[i] == ' ')
            {
                if (temp != "")
                {
                    seen.push_back(temp);
                    temp = "";
                }
            }
            else
            {
                temp += input[i];
            }
            i++;
        }
        if (temp != "")
        {
            seen.push_back(temp);
        }
        command = seen[0];
        args = std::vector(seen.begin() + 1, seen.end());
        return true;
    }

    void CommandProcessorImpl::process(const Command &command, Args args)
    {
        if (command == "")
        {
            return;
        }
        if (command == "help")
        {
            help();
            return;
        }
        if (command == "exit")
        {
            isRunning = false;
            return;
        }
        if (command == "clear")
        {
            clearScreen(true);
            return;
        }
        if (command == "history")
        {
            std::cout << d_history.getAllHistory();
            return;
        }
        auto res = validateArgs(command, args);
        if (!res.first)
        {
            throw std::invalid_argument(res.second);
        }
        if (d_commandProcessorMap.find(command) == d_commandProcessorMap.end())
        {
            throw std::invalid_argument("Command " + command + " not found");
        }
        d_commandProcessorMap[command](args);
    }

    void CommandProcessorImpl::clearScreen(bool enable)
    {
        //disable the clear functionality when needed, to allow flexibility.
        if (!enable) return;
        std::cout << "\033[2J\033[H";
        std::cout << "\033[2J\033[H";
    }

    std::pair<bool, std::string> CommandProcessorImpl::validateArgs(const Command &command, Args &args)
    {
        if (auto it = d_commandRuleMap.find(command); it == d_commandRuleMap.end())
        {
            return {true, ""};
        }

        std::string message = "";
        for (auto rule : d_commandRuleMap[command])
        {
            auto res = rule->apply(args);
            message += res.second;
            if (!res.first)
            {
                return {false, message};
            }
        }

        return {true, message};
    }

    std::string CommandProcessorImpl::getUserInput()
    {
        std::string currentInput = "";
        int pos = 0;
        std::string prompt = addColor(d_name + " => ", Color::GREEN);
        History temp;
        temp.addFront(currentInput);

        while (true)
        {
            std::cout << ("\r\033[K" + prompt + currentInput) << std::flush;
            
            // move the cursor
            int stepsBack = currentInput.length() - pos;
            for (int i = 0; i < stepsBack; i++)
            {
                std::cout << "\033[D" << std::flush;
            }

            auto input = KeyboardInput::getInstance().getInput();
            
            // add ascii character to current string
            if (input.first == KeyboardInput::InputType::ASCII)
            {
                currentInput = (currentInput.substr(0, pos) + input.second + currentInput.substr(pos));
                temp.edit(currentInput);
                pos++;
            }
            // remove from current string
            else if (input.first == KeyboardInput::InputType::BACKSPACE && currentInput.length() > 0)
            {
                currentInput = currentInput.substr(0, pos - 1) + currentInput.substr(pos);
                temp.edit(currentInput);
                pos--;
            }
            // return complete user input
            else if (input.first == KeyboardInput::InputType::ENTER)
            {
                std::cout << "\n";
                pos = 0;
                if (currentInput != "")
                    break;
            }
            // move cursor left
            else if (input.first == KeyboardInput::InputType::ARROW_LEFT && pos > 0)
            {
                pos--;
            }
            // move cursor right
            else if (input.first == KeyboardInput::InputType::ARROW_RIGHT && pos < currentInput.length())
            {
                pos++;
            }
            // go to previous history
            else if (input.first == KeyboardInput::InputType::ARROW_UP)
            {
                /**
                 * check history for temporary history first
                 * if not then check the permanent history
                 */
                auto v = temp.getPrevious();

                if (v.first)
                {
                    currentInput = v.second;
                    pos = currentInput.length();
                }
                else
                {
                    auto d = d_history.getPrevious();
                    if (d.first)
                    {
                        currentInput = d.second;
                        pos = currentInput.length();
                        temp.addFront(currentInput);
                    }
                }
            }
            // Go to newer history.
            else if (input.first == KeyboardInput::InputType::ARROW_DOWN)
            {
                auto v = temp.getNext();

                if (v.first)
                {
                    currentInput = v.second;
                    pos = currentInput.length();
                }
            }
        }
        return currentInput;
    }
}

//