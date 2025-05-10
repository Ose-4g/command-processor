#include "command-processor.h"
#include "util.h"
#include <iostream>
#include <format>
#include <regex>
#include <exception>

namespace ose4g
{
    CommandProcessor::CommandProcessor(const std::string &name) : d_name(name) {}

    void CommandProcessor::help()
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
    void CommandProcessor::add(const Command &command, std::function<void(Args)> processor, const std::string &description)
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

    void CommandProcessor::run()
    {
        clearScreen();
        std::string input;
        while (isRunning)
        {
            std::cout << "\n"
                      << addColor(d_name + "=> ", Color::GREEN);
            std::getline(std::cin, input);
            Command command;
            Args args;
            if (!parseStatement(input, command, args))
            {
                std::cout << addColor("Invalid input", Color::RED) << std::endl;
                continue;
            }
            try
            {
                if (!process(command, args))
                {
                    std::cout << addColor("Command not found", Color::RED) << std::endl;
                    continue;
                }
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

    bool CommandProcessor::parseStatement(const std::string &input, Command &command, Args &args)
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
    
    bool CommandProcessor::process(const Command &command, Args args)
    {
        if (command == "")
        {
            return true;
        }
        if (command == "help")
        {
            help();
            return true;
        }
        if (command == "exit")
        {
            isRunning = false;
            return true;
        }
        if (command == "clear")
        {
            clearScreen();
            return true;
        }
        if (d_commandProcessorMap.find(command) == d_commandProcessorMap.end())
        {
            return false;
        }
        d_commandProcessorMap[command](args);
        return true;
    }

    void CommandProcessor::clearScreen()
    {
        std::cout << "\033[2J\033[H";
    }
}
