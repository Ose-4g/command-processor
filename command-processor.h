#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H


#include <unordered_map>
#include <map>
#include <functional>
#include <string>

namespace ose4g
{
    using Args = std::vector<std::string>;
    using Command = std::string;

    class CommandProcessor
    {
    private:
        std::unordered_map<Command, std::function<void(Args)>> d_commandProcessorMap;
        std::map<Command, std::string> d_commandDescriptionMap;
        std::string d_name;
        bool isRunning  = true;
        void clearScreen();


    public:
        /**
         * @brief Constructor
         * 
         * @param name name of the command processor
         */
        CommandProcessor(const std::string &name);

        /**
         * @brief prints to the log details about each command.
         */
        void help();

        /**
         * @brief adds a new command.
         *
         * @param command Command string.
         * @param processor function to process the command
         * @param description description of command.
         * 
         * @returns boolean telling if add was successful or not.
         */
        bool add(const Command &command, std::function<void(Args)> processor, const std::string &description = "");

        /**
         * @brief starts the command processor process
         */
        void run();

        /**
         * @brief parses user input
         *
         * @param input user input.
         * @param command output command.
         * @param args arguments
         *
         * @returns boolean telling if parse was successful or not.
         */
        bool parseStatement(const std::string &input, Command &command, Args &args);

        /**
         * @brief processes a command given its args
         *
         * @param command the command
         * @param args the arguments to be processed with the command
         * 
         * @returns boolean telling if process was successful or not.
         */
        bool process(const Command &command, Args args);
    };
}

#endif