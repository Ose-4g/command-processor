#ifndef COMMAND_PROCESSOR_H
#define COMMAND_PROCESSOR_H

#include <unordered_map>
#include <map>
#include <functional>
#include <string>
#include "history.h"
namespace ose4g
{
    using Args = std::vector<std::string>;
    using Command = std::string;

    class Rule
    {
    public:
        virtual std::pair<bool, std::string> apply(ose4g::Args &args) = 0;
    };

    template <int MIN_ARG_COUNT = 1, int MAX_ARG_COUNT = 10>
    class ArgCountRule : public Rule
    {
    public:
        std::pair<bool, std::string> apply(Args &args)
        {
            bool valid = MIN_ARG_COUNT <= args.size() && args.size() <= MAX_ARG_COUNT;
            std::string message = "";
            if (!valid)
            {
                message = "Number of arguments should be between " + std::to_string(MIN_ARG_COUNT) + " and " + std::to_string(MAX_ARG_COUNT) + " But got " + std::to_string(args.size());
            }
            return {valid, message};
        }
    };

    class UserRule : public Rule
    {
    private:
        std::function<std::pair<bool, std::string>(const Args &)> f;

    public:
        UserRule(std::function<std::pair<bool, std::string>(const Args &)> func) : f(func) {}
        std::pair<bool, std::string> apply(Args &args)
        {
            return f(args);
        }
    };

    class CommandProcessorImpl
    {
    private:
        std::unordered_map<Command, std::function<void(const Args &)>> d_commandProcessorMap;
        std::map<Command, std::string> d_commandDescriptionMap;
        std::unordered_map<Command, std::vector<Rule *>> d_commandRuleMap;
        std::string d_name;
        bool isRunning = true;
        History d_history;

        // private methods
        void clearScreen();
        std::pair<bool, std::string> validateArgs(const Command &command, Args &args);
        std::string getUserInput();

    public:
        /**
         * @brief Constructor
         *
         * @param name name of the command processor
         */
        CommandProcessorImpl(const std::string &name);

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
         * command should be a string that meets the following requirements
         * - command starts with an alphabet
         * - command has only alphanumeric characters or -
         */
        void add(const Command &command, std::function<void(const Args &)> processor, const std::string &description = "");

        /**
         * @brief adds a new command.
         *
         * @param command Command string.
         * @param processor function to process the command
         * @param validateRules rules to validate the arguments
         * @param description description of command.
         *
         * command should be a string that meets the following requirements
         * - command starts with an alphabet
         * - command has only alphanumeric characters or -
         */
        void add(const Command &command, std::function<void(const Args &)> processor, const std::vector<Rule *> &validateRules, const std::string &description = "");

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
         */
        void process(const Command &command, Args args);
    };

    class CommandProcessor : public CommandProcessorImpl
    {
    private:
        using CommandProcessorImpl::help;
        using CommandProcessorImpl::parseStatement;
        using CommandProcessorImpl::process;

    public:
        /**
         * @brief Constructor
         *
         * @param name name of the command processor
         */
        CommandProcessor(const std::string &name) : CommandProcessorImpl(name) {}
    };
}

#endif