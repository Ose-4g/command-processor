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
                std::cout<<std::endl;
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
        auto res = validateArgs(command, args);
        if (!res.first)
        {
            throw std::invalid_argument(res.second);
        }
        if (d_commandProcessorMap.find(command) == d_commandProcessorMap.end())
        {
            throw std::invalid_argument("Command not found");
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
        int length = 0;

        while(true)
        {   
            std::cout<<("\r\033[K"+prompt +  currentInput)<<std::flush;

            // std::cout<<"\r"<<std::string(pos + prompt.size(),'\033[C')<<std::flush;
            auto input = KeyboardInput::getInstance().getInput();

            if(input.first == KeyboardInput::InputType::ASCII)
            {
                currentInput += input.second;
            }
            else if(input.first == KeyboardInput::InputType::BACKSPACE && currentInput.length() > 0)
            {
                currentInput.pop_back();
            }
            else if(input.first == KeyboardInput::InputType::ENTER)
            {
                std::cout<<"\n";
                if(currentInput!="") break;
            }
        }
        return currentInput;
    }
}
