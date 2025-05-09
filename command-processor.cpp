#include "command-processor.h"
#include <iostream>
#include <format>
#include <regex>

namespace ose4g{ 
    CommandProcessor::CommandProcessor(const std::string &name):d_name(name){}

    void CommandProcessor::help(){}
    bool CommandProcessor::add(const Command &command, std::function<void(Args)> processor, const std::string &description){
        // starts with alphabet. 
        // has alphanumeric characters or -
        std::regex commandPattern("^[A-Za-z][A-Za-z0-9-]*$"); 
        if(!std::regex_match(command, commandPattern))
        {
            return false;
        }
        d_commandProcessorMap[command] = processor;
        d_commandDescriptionMap[command] = description;
        return true;
    }

    void CommandProcessor::run(){
        clearScreen();
        std::string input;
        while(isRunning){
            std::cout<<"\033[1;32m"<<d_name<<"=> "<<"\033[0m";
            std::getline(std::cin, input);
        }
    }

    bool CommandProcessor::parseStatement(const std::string &input, Command &command, Args &args){
        if(input == "")
        {
            return true;
        }

        std::vector<std::string> seen;
        std::string temp = "";
        int i = 0; int n = input.size();
        while(i < n)
        {
            if(input[i] == '"' || input[i]=='\'')
            {
                int start = i;
                char c = input[i];
                i++;
                while(i < n && input[i]!=c)
                {
                    i++;
                }
                if(i == n)
                {
                    return false;
                }
                seen.push_back(input.substr(start+1, i-start-1));
            }

            else if(input[i]==' ')
            {
                if(temp!="")
                {
                    seen.push_back(temp);
                    temp="";
                }
            }else{
                temp += input[i];
            }
            i++;
        }
        if(temp!="")
        {
            seen.push_back(temp);
        }
        command = seen[0];
        args = std::vector(seen.begin()+1, seen.end());
        return true;
    }
    bool CommandProcessor::process(const Command &command, Args args){
        if(d_commandProcessorMap.find(command) == d_commandProcessorMap.end())
        {
            return false;
        }
        d_commandProcessorMap[command](args);
        return true;
    }

    void CommandProcessor::clearScreen(){
        std::cout << "\033[2J\033[H";
    }
}
