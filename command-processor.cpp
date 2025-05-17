#include "command-processor.h"
#include "util.h"
#include <iostream>
#include <format>
#include <regex>
#include <exception>
#include "keyboardinput.h"

namespace ose4g
{
    CommandProcessorImpl::CommandProcessorImpl(const std::string &name) : d_name(name) {}

    void CommandProcessorImpl::help()
    {
        std::string helpMessage = "\t" + addColor("help", Color::BLUE) + ": lists all commands and their description";
        helpMessage += "\n\t" + addColor("clear", Color::BLUE) + ": clear screen";
        helpMessage += "\n\t" + addColor("exit", Color::BLUE) + ": exit program";
        helpMessage += "\n\t" + addColor("hist", Color::BLUE) + ": print history";
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
        KeyboardInput::getInstance().enableKeyboard();
        clearScreen();
        std::string input;
        while (isRunning)
        {
            input = getUserInput();
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

            KeyboardInput::getInstance().disableKeyboard();
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
            clearScreen();
            return;
        }
        if (command == "hist")
        {
            std::cout << d_history.getAllHistory() << std::endl;
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

    void CommandProcessorImpl::clearScreen()
    {
        std::cout << "\033[2J\033[H";
    }

    std::pair<bool, std::string> CommandProcessorImpl::validateArgs(const Command &command, Args &args)
    {
        if (!d_commandRuleMap.count(command))
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
            int stepsBack = currentInput.length() - pos;
            for (int i = 0; i < stepsBack; i++)
            {
                std::cout << "\033[D" << std::flush;
            }

            auto input = KeyboardInput::getInstance().getInput();

            if (input.first == KeyboardInput::InputType::ASCII)
            {
                currentInput = (currentInput.substr(0, pos) + input.second + currentInput.substr(pos));
                temp.edit(currentInput);
                pos++;
            }
            else if (input.first == KeyboardInput::InputType::BACKSPACE && currentInput.length() > 0)
            {
                currentInput = currentInput.substr(0, pos - 1) + currentInput.substr(pos);
                temp.edit(currentInput);
                pos--;
            }
            else if (input.first == KeyboardInput::InputType::ENTER)
            {
                std::cout << "\n";
                pos = 0;
                if (currentInput != "")
                    break;
            }
            else if (input.first == KeyboardInput::InputType::ARROW_LEFT && pos > 0)
            {
                pos--;
            }
            else if (input.first == KeyboardInput::InputType::ARROW_RIGHT && pos < currentInput.length())
            {
                pos++;
            }
            else if (input.first == KeyboardInput::InputType::ARROW_UP)
            {
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